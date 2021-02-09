import numpy as np
import random

SNAKE_OFFSET=2
ACTION=4


class Snake:
	def __init__(self,nSize):
		self.nSize=nSize
		self.nLen=nSize*nSize
		self.Reset()

	def IndexToPos(self,index):
		return index//self.nSize,index%self.nSize

	def PosToIndex(self,pos):
		return pos[0]*self.nSize+pos[1]

	def Reset(self):
		self.dead=False
		self.map=np.zeros((self.nSize,self.nSize),dtype=int)
		start=random.randint(0,self.nSize-1)
		self.food=random.randint(0,self.nSize-1)
		while self.food==start:
			self.food=random.randint(0,self.nSize-1)
		self.snake=[start,]
		self.map[self.IndexToPos(start)]=1
		self.map[self.IndexToPos(self.food)]=-1

	def IsTerminated(self):
		return self.dead or len(self.snake)==self.nLen

	def GetObservation(self):
		ret=np.ones((self.nLen+SNAKE_OFFSET,),dtype=int)*-1
		if self.dead:
			ret[0]=1
		else:
			ret[0]=0
		ret[1]=self.food
		for i in range(len(self.snake)):
			ret[i+SNAKE_OFFSET]=self.snake[i]
		return ret

	def Step(self,action):
		pos=self.IndexToPos(self.snake[0])
		if action==0:
			if pos[0]<=0:
				self.dead=True
				return -self.nLen
			npos=(pos[0]-1,pos[1])
		elif action==1:
			if pos[1]>=(self.nSize-1):
				self.dead=True
				return -self.nLen
			npos=(pos[0],pos[1]+1)
		elif action==2:
			if pos[0]>=(self.nSize-1):
				self.dead=True
				return -self.nLen
			npos=(pos[0]+1,pos[1])
		elif action==3:
			if pos[1]<=0:
				self.dead=True
				return -self.nLen
			npos=(pos[0],pos[1]-1)
		else:
			return 0
		if self.map[npos]>0 and (self.map[npos]!=len(self.snake) or self.map[npos]==2):
			self.dead=True
			return -self.nLen
		nindex=self.PosToIndex(npos)
		if nindex==self.food:
			self.snake.insert(0,nindex)
			reward=1
			if len(self.snake)==self.nLen:
				self.food=-1
			else:
				while self.map[self.IndexToPos(self.food)]!=0:
					self.food=random.randint(0,self.nLen-1)
		else:
			reward=0
			for i in range(len(self.snake)-1,0,-1):
				self.snake[i]=self.snake[i-1]
			self.snake[0]=nindex
		self.map=np.zeros((self.nSize,self.nSize),dtype=int)
		if self.food>=0:
			self.map[self.IndexToPos(self.food)]=-1
		for i,s in enumerate(self.snake):
			self.map[self.IndexToPos(s)]=i+1
		return reward

	def Clone(self):
		ret=Snake(self.nSize)
		ret.dead=self.dead
		ret.food=self.food
		ret.snake=self.snake[:]
		ret.map=np.array(self.map)
		return ret

	def Print(self):
		shape=self.map.shape
		for i in range(shape[0]):
			print(self.map[i])

def GetAllInitialSnake(nSize):
	results=[]
	nLen=nSize*nSize
	for i in range(nLen):
		for j in range(nLen):
			if i==j:
				continue
			env=Snake(nSize)
			env.snake=[i,]
			env.food=j
			env.map=np.zeros((nSize,nSize),dtype=int)
			env.map[env.IndexToPos(i)]=1
			env.map[env.IndexToPos(j)]=-1
			results.append(env)
	return results