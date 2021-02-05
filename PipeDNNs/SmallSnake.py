import numpy as np
import tensorflow as tf
import PipeDNNs

INPUT_SIZE=11
OUTPUT_SIZE=4

class SnakeNN(PipeDNNs.BaseNNPipe):
	def __init__(self):
		self.model=tf.keras.Sequential([
			tf.keras.layers.InputLayer(input_shape=(INPUT_SIZE,)),
			tf.keras.layers.Dense(100,activation='relu'),
			tf.keras.layers.Dense(100,activation='relu'),
			tf.keras.layers.Dense(100,activation='relu'),
			tf.keras.layers.Dense(100,activation='relu'),
			tf.keras.layers.Dense(100,activation='relu'),
			tf.keras.layers.Dense(100,activation='relu'),
			tf.keras.layers.Dense(100,activation='relu'),
			tf.keras.layers.Dense(100,activation='relu'),
			tf.keras.layers.Dense(100,activation='relu'),
			tf.keras.layers.Dense(100,activation='relu'),
			tf.keras.layers.Dense(OUTPUT_SIZE)
			])
		self.model.compile(optimizer=tf.keras.optimizers.Adam(1e-4),loss='mse')
		self.train_data=[]

	def GetInputShape(self):
		return [INPUT_SIZE,]

	def GetOutputShape(self):
		return [OUTPUT_SIZE,]
	
	def AddTrainingData(self,x,y):
		print('got training data:')
		print(x)
		print(y)
		self.train_data.append((x,y))

	def Train(self):
		print("Training started!")
		x=np.array([d[0] for d in self.train_data])
		y=np.array([d[1] for d in self.train_data])
		self.model.fit(x,y)
		self.train_data=[]

	def Predict(self,x):
		print(x)
		return self.model.predict(np.array([x,]))

def main():
	print('start nn pipe')
	nn=SnakeNN()
	nn.StartPipe('\\\\.\\pipe\\LiudadaNemaedPipe')
	print('the end')

if __name__=='__main__':
	main()
