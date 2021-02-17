
import gym
import tensorflow as tf
import numpy as np
import sys
import os
import random

def CreateNewModel(input_len,output_len):
	model = tf.keras.Sequential([tf.keras.layers.InputLayer(input_shape=(input_len,)),
		tf.keras.layers.Dense(30,activation='relu'),
		tf.keras.layers.Dense(30,activation='relu'),
			tf.keras.layers.Dense(output_len,activation=tf.keras.activations.linear)])
	model.compile(optimizer=tf.keras.optimizers.Adam(1e-3),loss='mse')
	return model

BATCH_SIZE=64
GAMMA=0.95

class DDQN(object):
	def __init__(self,env):
		self.nLen=len(env.reset())
		self.nAction=env.action_space.n
		self.models=[CreateNewModel(self.nLen,self.nAction) for i in range(2)]#下标1的网络是目标网络
		self.experiments=set()


	def GetValues(self,observation,index=0):
		return self.models[index].predict(observation.reshape((1,self.nLen))).reshape((self.nAction,))

	def GetAction(self,observation,ebsilon,index=0):
		if random.random()<ebsilon:
			return random.randint(0,self.nAction-1)
		values=self.GetValues(observation,index)
		return np.argmax(values)

	def Save(self,filepath):#保存网络权值，并更新目标网络
		self.models[0].save_weights(filepath)
		#self.models[1].load_weights(filepath)
		#self.models[1].save(filepath2)

	def Load(self,filepath):
		try:
			self.models[0].load_weights(filepath)
			self.models[1].load_weights(filepath)
		except Exception as e:
			print('loading model weights failed:',repr(e))

	def DDQN(self,env,ebsilon):
		first=env.reset()
		bingo=0
		done=False
		while not done:
			action=self.GetAction(first,ebsilon,0)
			second, reward, done, info = env.step(action)
			if reward>0:
				bingo+=reward
			exp=(tuple(first),action,reward,tuple(second),done)
			if not exp in self.experiments:
				self.experiments.add(exp)
			if len(self.experiments)>=BATCH_SIZE:
				batch_x=[]
				batch_y=[]
				selected=random.sample(self.experiments,BATCH_SIZE)
				for s1,a,r,s2,d in selected:
					s1=np.array(s1)
					s2=np.array(s2)
					batch_x.append(s1)
					ys=self.GetValues(s1,0)
					if d:
						ys[a]=r
					else:
						na=self.GetAction(s2,0.0,0)
						nys=self.GetValues(s2,1)
						ys[a]=r+GAMMA*nys[na]
					batch_y.append(ys)
				self.models[0].fit(np.array(batch_x),np.array(batch_y),batch_size=BATCH_SIZE,epochs=1,verbose=1)
			first=second
		self.models[1].set_weights(self.models[0].get_weights())
		return bingo

	
def main():
	EPOCH=30000
	weight_file='cartpole.h5'
	env=gym.make('CartPole-v1')
	dqn=DDQN(env)
	if len(sys.argv)>1:
		EPOCH=int(sys.argv[1])
	if len(sys.argv)>2:
		weight_file=sys.argv[2]
		if len(weight_file)<3 or weight_file[-3:]!='.h5':
			weight_file+='.h5'
		dqn.Load(sys.argv[2])
	else:
		dqn.Save(weight_file)
	if len(sys.argv)>3:
		log_file=sys.argv[3]
	maxb=0
	allb=0
	for i in range(EPOCH):
		b=dqn.DDQN(env,0.01)
		allb+=b
		if b>maxb:
			maxb=b
			print('got progress[%d] at %d'%(b,i))
		if (i%10)==9:
			dqn.Save(weight_file)
			os.system("cls")
			print('epoch %d finished!max progress:%d,average:%f'%(i,maxb,allb/10))
			if log_file!=None:
				with open(log_file,'a') as fp:
					fp.write('epoch %d,max progress:%d,average:%f\n'%(i,maxb,allb/10))
			allb=0


if __name__=='__main__':
	main()