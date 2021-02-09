import numpy as np
import random
import sys
import PySnakeAI

class TDLearn(object):
	def __init__(self,naction=4):
		self.judger=[{},{}]
		self.naction=naction


	def GetValues(self,observation,index=0):
		obs=tuple(observation)
		if obs in self.judger[index].keys():
			return self.judger[index][obs]
		return np.zeros((self.naction,))

	def GetAction(self,observation,ebsilon,index=0):
		if random.random()<ebsilon:
			return random.randint(0,self.naction-1)
		if index<0:
			values=np.zeros((self.naction,))
			for i in range(len(self.judger)):
				values+=self.GetValues(observation,i)
		else:
			values=self.GetValues(observation,index)
		return np.argmax(values)

	def UpdateValue(self,observation,action,value,lr=0.1,index=0):
		values=self.GetValues(observation)
		values[action]+=lr*(value-values[action])
		obs=tuple(observation)
		self.judger[index][obs]=values

	def DoubleQLearn(self,env:PySnakeAI.Snake,ebsilon=0.1):
		bingo=0
		while not env.IsTerminated():
			s1=env.GetObservation()
			a=self.GetAction(s1,ebsilon,-1)
			r=env.Step(a)
			index=random.randint(0,1)
			if r>0:
				bingo+=r
			if env.IsTerminated():
				u=r
			else:
				s2=env.GetObservation()
				na=self.GetAction(s2,0.0,index)
				values=self.GetValues(s2,1-index)
				u=r+values[na]
			self.UpdateValue(s1,a,u,0.1,index)
		return bingo

def main():
	SIZE=3
	EPOCH=300
	if len(sys.argv)>1:
		SIZE=int(sys.argv[1])
	if len(sys.argv)>2:
		EPOCH=int(sys.argv[2])
	td=TDLearn(PySnakeAI.ACTION)
	maxb=0
	for i in range(EPOCH):
		envs=PySnakeAI.GetAllInitialSnake(SIZE)
		bingo=0
		for env in envs:
			env=PySnakeAI.Snake(SIZE)
			b=td.DoubleQLearn(env)
			if b==(SIZE*SIZE-1):
				bingo+=1
			if b>maxb:
				maxb=b
				print('got progress[%d] at %d'%(b,i))
		if bingo>0:
			print('epoch %d reached final %d times'%(i,bingo))

	print('test:')
	env=PySnakeAI.Snake(SIZE)
	env.Print()
	while not env.IsTerminated():
		a=td.GetAction(env.GetObservation(),0.0)
		env.Step(a)
		env.Print()

if __name__=='__main__':
	main()