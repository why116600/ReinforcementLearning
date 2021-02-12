import tensorflow as tf
import numpy as np
import PySnakeAI
import random
import sys
import os

class BaselineVPG(object):
	def __init__(self,nSize,nAction):
		self.nSize=nSize
		self.nLen=nSize*nSize
		self.nInput=(self.nLen+1)*(self.nLen+1)
		self.nAction=nAction
		self.policy_net=tf.keras.Sequential([tf.keras.layers.InputLayer(input_shape=(self.nInput,)),
			tf.keras.layers.Dense(900,activation='relu'),
			tf.keras.layers.Dense(900,activation='relu'),
			tf.keras.layers.Dense(900,activation='relu'),
			tf.keras.layers.Dense(self.nAction,activation=tf.nn.softmax)])
		self.state_net=tf.keras.Sequential([tf.keras.layers.InputLayer(input_shape=(self.nInput,)),
			tf.keras.layers.Dense(900,activation='relu'),
			tf.keras.layers.Dense(900,activation='relu'),
			tf.keras.layers.Dense(900,activation='relu'),
			tf.keras.layers.Dense(1,activation=tf.keras.activations.linear)])

		self.policy_net.compile(optimizer=tf.keras.optimizers.Adam(1e-4),loss=tf.keras.losses.categorical_crossentropy)
		self.state_net.compile(optimizer=tf.keras.optimizers.Adam(1e-4),loss='mse')

	def Preprocess(self,observation):#使用独热编码预处理
		result=[]
		for i,v in enumerate(observation[1:]):
			onehot=[0,]*(self.nLen+1)
			onehot[v+1]=1
			result+=onehot
		return np.array([result,])

	def GetValue(self,observation):#获取状态价值
		obs=self.Preprocess(observation)
		values=self.state_net.predict(obs)
		return values[0,0]

	def GetAction(self,observation):#动作决策
		obs=self.Preprocess(observation)
		probs=self.policy_net.predict(obs)[0]
		return np.random.choice(self.nAction,p=probs)

	def Learn(self,env:PySnakeAI.Snake):
		trace=[]
		bingo=0
		while not env.IsTerminated():
			first=env.GetObservation()
			action=self.GetAction(first)
			reward=env.Step(action)
			if reward>0:
				bingo+=reward
			if reward<0:
				reward=0
			done=env.IsTerminated()
			trace.append((first,action,reward,done))
		all_rewards=0
		states=[]
		values=[]
		aps=[]
		for s1,a,r,done in trace[::-1]:
			states.append(self.Preprocess(s1).reshape((self.nInput,)))
			all_rewards+=r
			values.append([all_rewards,])
			ap=np.zeros((self.nAction,))
			ap[a]=(all_rewards-self.GetValue(s1))
			aps.append(ap)
		self.state_net.fit(np.array(states),np.array(values),batch_size=len(trace))
		self.policy_net.fit(np.array(states),np.array(aps),batch_size=len(trace))
		return bingo


		
def main():
	SIZE=3
	EPOCH=30000
	weight_file='vpg_model.h5'
	if len(sys.argv)>1:
		SIZE=int(sys.argv[1])
	if len(sys.argv)>2:
		EPOCH=int(sys.argv[2])
	student=BaselineVPG(SIZE,PySnakeAI.ACTION)
	if len(sys.argv)>3:
		weight_file=sys.argv[3]
		#dqn.Load(sys.argv[3])
	#else:
		#dqn.Save(weight_file)
	maxb=0
	for i in range(EPOCH):
		envs=PySnakeAI.GetAllInitialSnake(SIZE)
		bingo=0
		allb=0
		for j,env in enumerate(envs):
			env=PySnakeAI.Snake(SIZE)
			b=student.Learn(env)
			allb+=b
			if b==(SIZE*SIZE-1):
				bingo+=1
			if b>maxb:
				maxb=b
				print('got progress[%d] at %d'%(b,i))
		os.system("cls")
		#dqn.Save(weight_file)
		print('epoch %d finished!max progress:%d,average:%f'%(i,maxb,allb/len(envs)))
		with open('train.log','a') as fp:
			fp.write('epoch %d,max progress:%d,average:%f\n'%(i,maxb,allb/len(envs)))
		if bingo>0:
			print('epoch %d reached final %d times'%(i,bingo))


if __name__=='__main__':
	main()