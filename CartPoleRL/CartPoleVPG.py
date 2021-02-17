import tensorflow as tf
import numpy as np
import gym
import random
import sys
import os

GAMMA=0.95

class BaselineVPG(object):
	def __init__(self,env):
		self.nLen=len(env.reset())
		self.nAction=env.action_space.n
		self.nInput=self.nLen
		self.policy_net=tf.keras.Sequential([tf.keras.layers.InputLayer(input_shape=(self.nInput,)),
			tf.keras.layers.Dense(40,activation='relu'),
			tf.keras.layers.Dense(40,activation='relu'),
			tf.keras.layers.Dense(self.nAction,activation=tf.nn.softmax)])
		self.state_net=tf.keras.Sequential([tf.keras.layers.InputLayer(input_shape=(self.nInput,)),
			tf.keras.layers.Dense(40,activation='relu'),
			tf.keras.layers.Dense(40,activation='relu'),
			tf.keras.layers.Dense(1,activation=tf.keras.activations.linear)])

		self.policy_net.compile(optimizer=tf.keras.optimizers.Adam(1e-3),loss=tf.keras.losses.categorical_crossentropy)
		self.state_net.compile(optimizer=tf.keras.optimizers.Adam(1e-3),loss='mse')


	def GetValue(self,observation):#获取状态价值
		obs=observation.reshape(1,self.nInput)
		values=self.state_net.predict(obs)
		return values[0,0]

	def GetAction(self,observation):#动作决策
		obs=observation.reshape(1,self.nInput)
		probs=self.policy_net.predict(obs)[0]
		return np.random.choice(self.nAction,p=probs)

	def Learn(self,env):
		trace=[]
		bingo=0
		done=False
		first=env.reset()
		account=1.0
		while not done:
			action=self.GetAction(first)
			second, reward, done, info=env.step(action)
			if reward>0:
				bingo+=reward
			trace.append((first,action,reward,done,account))
			account*=GAMMA
			first=second
		all_rewards=0
		for s1,a,r,done,account in trace[::-1]:
			all_rewards=all_rewards*GAMMA+r
			self.state_net.fit(s1[np.newaxis],np.ones((1,1))*all_rewards,verbose=0)
			td_error=all_rewards-self.state_net.predict(s1[np.newaxis])
			x_tensor = tf.convert_to_tensor(s1[np.newaxis],dtype=tf.float32)
			with tf.GradientTape() as tape:
				pi_tensor = self.policy_net(x_tensor)[0, a]
				logpi_tensor = tf.math.log(tf.clip_by_value(pi_tensor,1e-6, 1.))
				loss_tensor = -account * td_error * logpi_tensor
			grad_tensors = tape.gradient(loss_tensor, self.policy_net.variables)
			self.policy_net.optimizer.apply_gradients(zip(grad_tensors, self.policy_net.variables))
		return bingo
	
	def Save(self,filename):
		self.policy_net.save(filename+'-p.h5')
		self.state_net.save(filename+'-s.h5')

	def Load(self,filename):
		try:
			pn=tf.keras.models.load_model(filename+'-p.h5')
			sn=tf.keras.models.load_model(filename+'-s.h5')
			self.policy_net=pn
			self.state_net=sn
		except BaseException as e:
			print('loading failed:',repr(e))
	
def main():
	EPOCH=30000
	weight_file='cartpole_vpg'
	env=gym.make('CartPole-v1')
	agent=BaselineVPG(env)
	if len(sys.argv)>1:
		EPOCH=int(sys.argv[1])
	if len(sys.argv)>2:
		agent.Load(sys.argv[2])
		weight_file=sys.argv[2]
	#else:
		#agent.Save(weight_file)
	if len(sys.argv)>3:
		log_file=sys.argv[3]
	maxb=0
	allb=0
	for i in range(EPOCH):
		b=agent.Learn(env)
		allb+=b
		if b>maxb:
			maxb=b
			print('got progress[%d] at %d'%(b,i))
		if (i%10)==9:
			agent.Save(weight_file)
			#os.system("cls")
			print('epoch %d finished!max progress:%d,average:%f'%(i,maxb,allb/10))
			if log_file!=None:
				with open(log_file,'a') as fp:
					fp.write('epoch %d,max progress:%d,average:%f\n'%(i,maxb,allb/10))
			allb=0


if __name__=='__main__':
	main()