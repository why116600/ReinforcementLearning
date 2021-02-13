import tensorflow as tf
import numpy as np
import PySnakeAI
import random
import sys
import os

def CreateNewModel(input_len,output_len):
	model = tf.keras.Sequential([tf.keras.layers.InputLayer(input_shape=(input_len,)),
		tf.keras.layers.Dense(300,activation='relu'),
		tf.keras.layers.Dense(300,activation='relu'),
		#tf.keras.layers.Dense(900,activation='relu'),
		#tf.keras.layers.Dense(900,activation='relu'),
		#tf.keras.layers.Dense(900,activation='relu'),
		#tf.keras.layers.Dense(900,activation='relu'),
			tf.keras.layers.Dense(output_len,activation=tf.keras.activations.linear)])
	model.compile(optimizer=tf.keras.optimizers.Adam(1e-3),loss='mse')
	return model

BATCH_SIZE=64

class DDQN(object):
	def __init__(self,nSize):
		self.nSize=nSize
		self.nLen=nSize*nSize
		self.models=[CreateNewModel((self.nLen+1)*(self.nLen+1),PySnakeAI.ACTION) for i in range(2)]#下标1的网络是目标网络
		self.experiments=set()

	def Preprocess(self,observation):#使用独热编码预处理
		result=[]
		for i,v in enumerate(observation[1:]):
			onehot=[0,]*(self.nLen+1)
			onehot[v+1]=1
			result+=onehot
		return np.array([result,])

	def GetValues(self,observation,index=0):
		obser=self.Preprocess(observation)
		return self.models[index].predict(obser).reshape((PySnakeAI.ACTION,))

	def GetAction(self,observation,ebsilon,index=0):
		if random.random()<ebsilon:
			return random.randint(0,PySnakeAI.ACTION-1)
		values=self.GetValues(observation,index)
		return np.argmax(values)

	def Save(self,filepath):#保存网络权值，并更新目标网络
		self.models[0].save_weights(filepath)
		self.models[1].load_weights(filepath)
		#self.models[1].save(filepath2)

	def Load(self,filepath):
		try:
			self.models[0].load_weights(filepath)
			self.models[1].load_weights(filepath)
		except Exception as e:
			print('loading model weights failed:',repr(e))

	def DDQN(self,env:PySnakeAI.Snake,ebsilon):
		first=env.GetObservation()
		bingo=0
		while not env.IsTerminated():
			action=self.GetAction(first,ebsilon,0)
			reward=env.Step(action)
			if reward>0:
				bingo+=reward
			second=env.GetObservation()
			exp=(tuple(first),action,reward,tuple(second))
			if not exp in self.experiments:
				self.experiments.add(exp)
			if len(self.experiments)>=BATCH_SIZE:
				batch_x=[]
				batch_y=[]
				selected=random.sample(self.experiments,BATCH_SIZE)
				for s1,a,r,s2 in selected:
					batch_x.append(self.Preprocess(s1))
					ys=self.GetValues(s1,0)
					if s1[0]==1:
						ys[a]=r
					else:
						na=self.GetAction(s2,0.0,0)
						nys=self.GetValues(s2,1)
						ys[a]=r+nys[na]
					batch_y.append(ys)
				self.models[0].fit(np.array(batch_x),np.array(batch_y),batch_size=BATCH_SIZE,epochs=1,verbose=1)
			first=second
		self.models[1].set_weights(self.models[0].get_weights())
		return bingo


def main():
	SIZE=3
	EPOCH=30000
	weight_file='snake_model.h5'
	if len(sys.argv)>1:
		SIZE=int(sys.argv[1])
	if len(sys.argv)>2:
		EPOCH=int(sys.argv[2])
	dqn=DDQN(SIZE)
	if len(sys.argv)>3:
		dqn.Load(sys.argv[3])
		weight_file=sys.argv[3]
	else:
		dqn.Save(weight_file)
	maxb=0
	for i in range(EPOCH):
		envs=PySnakeAI.GetAllInitialSnake(SIZE)
		bingo=0
		allb=0
		for j,env in enumerate(envs):
			env=PySnakeAI.Snake(SIZE)
			b=dqn.DDQN(env,0.1)
			allb+=b
			if (j%9)==0:
				dqn.Save(weight_file)
			if b==(SIZE*SIZE-1):
				bingo+=1
			if b>maxb:
				maxb=b
				print('got progress[%d] at %d'%(b,i))
		os.system("cls")
		dqn.Save(weight_file)
		print('epoch %d finished!max progress:%d,average:%f'%(i,maxb,allb/len(envs)))
		with open('train.log','a') as fp:
			fp.write('epoch %d,max progress:%d,average:%f\n'%(i,maxb,allb/len(envs)))
		if bingo>0:
			print('epoch %d reached final %d times'%(i,bingo))


if __name__=='__main__':
	main()