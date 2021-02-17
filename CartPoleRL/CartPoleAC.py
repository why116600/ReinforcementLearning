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
		self.action_net=tf.keras.Sequential([tf.keras.layers.InputLayer(input_shape=(self.nInput,)),
			tf.keras.layers.Dense(40,activation='relu'),
			#tf.keras.layers.Dense(30,activation='relu'),
			tf.keras.layers.Dense(self.nAction,activation=tf.nn.softmax)])
		self.critic_net=tf.keras.Sequential([tf.keras.layers.InputLayer(input_shape=(self.nInput,)),
			tf.keras.layers.Dense(40,activation='relu'),
			#tf.keras.layers.Dense(30,activation='relu'),
			tf.keras.layers.Dense(self.nAction,activation=tf.keras.activations.linear)])
		self.value_net=tf.keras.Sequential([tf.keras.layers.InputLayer(input_shape=(self.nInput,)),
			tf.keras.layers.Dense(40,activation='relu'),
			#tf.keras.layers.Dense(30,activation='relu'),
			tf.keras.layers.Dense(1,activation=tf.keras.activations.linear)])

		self.action_net.compile(optimizer=tf.keras.optimizers.Adam(1e-3),loss=tf.keras.losses.categorical_crossentropy)
		self.critic_net.compile(optimizer=tf.keras.optimizers.Adam(1e-3),loss='mse')
		self.value_net.compile(optimizer=tf.keras.optimizers.Adam(1e-3),loss='mse')


	def GetValue(self,observation,actionValue=True):#获取动作价值
		obs=observation.reshape(1,self.nInput)
		if actionValue:
			values=self.critic_net.predict(obs)
			return values[0]
		return self.value_net.predict(obs)[0]

	def GetAction(self,observation):#动作决策
		obs=observation.reshape(1,self.nInput)
		probs=self.action_net.predict(obs)[0]
		return np.random.choice(self.nAction,p=probs)

	def Learn(self,env):
		bingo=0
		done=False
		all_rewards=0
		first=env.reset()
		account=1.0
		while not done:
			value=self.GetValue(first,False)
			action=self.GetAction(first)
			second, reward, done, info=env.step(action)
			u=reward
			if not done:
				#na=self.GetAction(second)
				nv=self.GetValue(second,False)
				u+=GAMMA*nv[0]
			#ap=np.zeros((self.nAction,))
			#ap[action]=account*(u-value[0])
			td_error = u - self.value_net.predict(first[np.newaxis])
			value[0]=u
			x_tensor = tf.convert_to_tensor(first[np.newaxis],dtype=tf.float32)
			with tf.GradientTape() as tape:
				pi_tensor = self.action_net(x_tensor)[0, action]
				logpi_tensor = tf.math.log(tf.clip_by_value(pi_tensor,1e-6, 1.))
				loss_tensor = -account * td_error * logpi_tensor
			grad_tensors = tape.gradient(loss_tensor, self.action_net.variables)
			self.action_net.optimizer.apply_gradients(zip(grad_tensors, self.action_net.variables))
			self.value_net.fit(first[np.newaxis],value[np.newaxis],batch_size=1,verbose=0)
			if reward>0:
				bingo+=reward
			account*=GAMMA
			first=second
		#self.action_net.fit(np.array(states),np.array(aps),batch_size=1,verbose=0)
		return bingo
	
	def Save(self,filename):
		self.action_net.save(filename+'-a.h5')
		self.critic_net.save(filename+'-c.h5')
		self.value_net.save(filename+'-v.h5')

	def Load(self,filename):
		try:
			pn=tf.keras.models.load_model(filename+'-a.h5')
			sn=tf.keras.models.load_model(filename+'-c.h5')
			vn=tf.keras.models.load_model(filename+'-v.h5')
			self.action_net=pn
			self.critic_net=sn
			self.value_net=vn
		except BaseException as e:
			print('loading failed:',repr(e))
	
def main():
	EPOCH=30000
	weight_file='cartpole_ac'
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