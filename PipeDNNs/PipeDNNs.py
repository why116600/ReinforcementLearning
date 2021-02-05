import win32pipe
import win32file
import struct
import numpy as np

def ReadDataFromPipe(pipe,dataSize):
	data=b''
	while len(data)<dataSize:
		d=win32file.ReadFile(pipe,dataSize-len(data))
		data+=d[1]
	return data

def ReadIntFromPipe(pipe):
	data=ReadDataFromPipe(pipe,4)
	ret=struct.unpack('=i',data)
	return ret[0]

def ReadDoubleFromPipe(pipe):
	data=ReadDataFromPipe(pipe,8)
	ret=struct.unpack('=d',data)
	return ret[0]

def ReadArrayFromPipe(pipe):
	n=ReadIntFromPipe(pipe)
	arr=[]
	for i in range(n):
		x=ReadDoubleFromPipe(pipe)
		arr.append(x)
	return arr

def SendInt(pipe,code):
	data=struct.pack('=i',code)
	win32file.WriteFile(pipe,data)

def SendShape(pipe,msgType,shape):
	if shape==None:
		data=struct.pack('=ii',msgType,0)
		win32file.WriteFile(pipe,data)
	else:
		data=struct.pack('=ii',msgType,len(shape))
		#win32file.WriteFile(pipe,data)
		for i in range(len(shape)):
			data+=struct.pack('=i',shape[i])
		win32file.WriteFile(pipe,data)

def SendDoubleArray(pipe,arr):
	if type(arr)!=np.ndarray:
		win32file.WriteFile(pipe,struct.pack('=i',0))
		return
	data=struct.pack('=i',len(arr))
	for i in range(len(arr)):
		data+=struct.pack('=d',arr[i])
	win32file.WriteFile(pipe,data)

def GetShapeLength(shape):
	if shape==None:
		return 0
	ret=1
	for s in shape:
		ret*=s
	return ret


ErrNoNetwork=-1#没有有效的神经网络
ErrWrongShape=-2#数据形状错误

class BaseNNPipe:
	def __init__(self):
		pass

	def GetInputShape(self):
		return None
	
	def GetOutputShape(self):
		return None

	def AddTrainingData(self,x,y):
		pass

	def Train(self):
		pass

	def Predict(self,x):
		return None

	def StartPipe(self,strPipeName):
		bFinish=False
		pipe=win32pipe.CreateNamedPipe(strPipeName,\
			win32pipe.PIPE_ACCESS_DUPLEX,win32pipe.PIPE_TYPE_BYTE|win32pipe.PIPE_WAIT,\
			win32pipe.PIPE_UNLIMITED_INSTANCES,4096,4096,0,None)
		try:
			win32pipe.ConnectNamedPipe(pipe,None)
			while True:
				msgType=ReadIntFromPipe(pipe)
				if msgType==0:
					bFinish=True
					break
				elif msgType==1:#获取输入的张量形状
					shape=self.GetInputShape()
					SendShape(pipe,msgType,shape)
				elif msgType==2:#获取输出的张量形状
					shape=self.GetOutputShape()
					SendShape(pipe,msgType,shape)
				elif msgType==3:#添加训练集
					#获取输入数据
					in_arr=ReadArrayFromPipe(pipe)
					#获取标记数据
					co_arr=ReadArrayFromPipe(pipe)
					in_shp=self.GetInputShape()
					co_shp=self.GetOutputShape()
					if in_shp==None or co_shp==None:
						SendInt(pipe,ErrNoNetwork)
						continue
					if GetShapeLength(in_shp)!=len(in_arr) or GetShapeLength(co_shp)!=len(co_arr):
						SendInt(pipe,ErrWrongShape)
						continue
					self.AddTrainingData(np.array(in_arr).reshape(in_shp),np.array(co_arr).reshape(co_shp))
					SendInt(pipe,msgType)
				elif msgType==4:#开始训练
					self.Train()
					SendInt(pipe,msgType)
				elif msgType==5:#用网络进行预测
					in_arr=ReadArrayFromPipe(pipe)
					in_shp=self.GetInputShape()
					if GetShapeLength(in_shp)!=len(in_arr):
						SendInt(pipe,ErrWrongShape)
						continue
					y=self.Predict(np.array(in_arr).reshape(in_shp))
					out_len=GetShapeLength(self.GetOutputShape())
					SendInt(pipe,msgType)
					if type(y)==np.ndarray:
						SendDoubleArray(pipe,y.reshape((out_len,)))
					else:
						SendDoubleArray(pipe,None)
					

		except BaseException as e:
			print('pipe exception happened:',repr(e))
		finally:
			win32file.CloseHandle(pipe)
			print('Pipe ends!')
		return bFinish