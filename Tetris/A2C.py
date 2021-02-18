import tensorflow as tf
import numpy as np
import Tetris
import random
import sys
import os

GAMMA=0.99

def BuildNetwork(input_shape,output_len,activate_func,loss_func):
	ret_net=tf.keras.Sequential([
		tf.keras.layers.Conv2D(filters= 32, kernel_size=(3,3), padding='Same', activation='relu',input_shape=input_shape),
		tf.keras.layers.Conv2D(filters= 16, kernel_size=(3,3), padding='Same', activation='relu'),
		tf.keras.layers.MaxPooling2D(pool_size=(2, 2)),
		tf.keras.layers.Flatten(),
		tf.keras.layers.Dense(400,activation='relu'),
		#tf.keras.layers.Dense(30,activation='relu'),
		tf.keras.layers.Dense(output_len,activation=activate_func)])
	ret_net.compile(optimizer=tf.keras.optimizers.Adam(1e-3),loss=loss_func)
	return ret_net

class A2C(object):
	def __init__(self,env):
		self.nAction=env.nAction
		self.nInput=env.GetObservation().shape
		self.nLen=self.nInput[-1]*self.nInput[-2]
		self.action_net=BuildNetwork(self.nInput,self.nAction,tf.keras.activations.softmax,tf.keras.losses.categorical_crossentropy)
		self.value_net=BuildNetwork(self.nInput,1,tf.keras.activations.linear,'mse')


	def GetValue(self,observation):#获取动作价值
		obs=observation[np.newaxis]
		return self.value_net.predict(obs)[0]

	def GetAction(self,observation):#动作决策
		obs=observation[np.newaxis]
		probs=self.action_net.predict(obs)[0]
		return np.random.choice(self.nAction,p=probs)

	def Learn(self,env):
		bingo=0
		repeat=0
		done=False
		all_rewards=0
		first=env.GetObservation()
		account=1.0
		while not done and repeat<=self.nLen*2:
			value=self.GetValue(first)
			action=self.GetAction(first)
			reward=env.Step(action)
			second=env.GetObservation()
			done=env.IsTerminated()
			u=reward
			if not done:
				nv=self.GetValue(second)
				u+=GAMMA*nv[0]
			s1=first[np.newaxis]
			td_error = u - self.value_net.predict(s1)
			value[0]=u
			x_tensor = tf.convert_to_tensor(s1,dtype=tf.float32)
			with tf.GradientTape() as tape:
				pi_tensor = self.action_net(x_tensor)[0, action]
				logpi_tensor = tf.math.log(tf.clip_by_value(pi_tensor,1e-6, 1.))
				loss_tensor = -account * td_error * logpi_tensor
			grad_tensors = tape.gradient(loss_tensor, self.action_net.variables)
			self.action_net.optimizer.apply_gradients(zip(grad_tensors, self.action_net.variables))
			self.value_net.fit(s1,value[np.newaxis],batch_size=1,verbose=0)
			repeat+=1
			if reward>0:
				bingo+=reward
				repeat=0
			account*=GAMMA
			first=second
		if repeat>self.nLen*2:
			print('repeat too much!')
		return bingo
	
	def Save(self,filename):
		self.action_net.save(filename+'-a.h5')
		self.value_net.save(filename+'-v.h5')

	def Load(self,filename):
		try:
			pn=tf.keras.models.load_model(filename+'-a.h5')
			vn=tf.keras.models.load_model(filename+'-v.h5')
			self.action_net=pn
			self.value_net=vn
		except BaseException as e:
			print('loading failed:',repr(e))
	
def main():
	EPOCH=30000
	weight_file='tetris'
	if len(sys.argv)>1:
		EPOCH=int(sys.argv[1])
	env=Tetris.Tetris(100)
	agent=A2C(env)
	if len(sys.argv)>2:
		agent.Load(sys.argv[2])
		weight_file=sys.argv[2]
	if len(sys.argv)>3:
		log_file=sys.argv[3]
	maxb=0
	bingo=0
	for i in range(EPOCH):
		env.Reset()
		b=agent.Learn(env)
		if b>maxb:
			maxb=b
			print('got progress[%d] at %d'%(b,i))
		agent.Save(weight_file)
			#os.system("cls")
		print('epoch %d finished!max progress:%d,score:%f'%(i,maxb,b))
		if log_file!=None:
			with open(log_file,'a') as fp:
				fp.write('epoch %d,max progress:%d,score:%f\n'%(i,maxb,b))
		bingo=0


if __name__=='__main__':
	main()