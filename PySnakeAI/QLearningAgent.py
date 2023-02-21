import numpy as np
import PySnakeAI
import random
import struct
import sys

SIZE=4

class QLearningAgent:
	def __init__(self,nEnvSize,gamma=0.9,lr=0.001):
		self.nSpaceSize=nEnvSize**2
		self.gamma=gamma
		self.learnRate=lr
		self.qtable={}
		self.maxstage=0

	def getSATuple(self,state,action):
		statenum=0
		stage=-1
		for i in range(1,len(state)):
			if state[i]<0:
				break
			statenum+=state[i]*(self.nSpaceSize**(i-1))
			stage+=1
		return (stage,statenum,action)

	def getAction(self,state,rigidity=1.0):
		bingo=False
		maxvalue=0.0
		action=random.randint(0,3)
		if random.random()>rigidity:
			return action
		for a in range(4):
			sat=self.getSATuple(state,a)
			if sat in self.qtable.keys():
				v=self.qtable[sat]
				if not bingo or maxvalue<v:
					maxvalue=v
					action=a
		return action

	def learn(self,env):
		state=env.GetObservation()
		action=self.getAction(state,0.8)
		reward=env.Step(action)
		sat=self.getSATuple(state,action)
		state_=env.GetObservation()
		if not sat in self.qtable.keys():
			self.qtable[sat]=0
		if env.IsTerminated() or sat[0]>=6:
			self.qtable[sat]+=self.learnRate*(reward-self.qtable[sat])
			env.Reset()
			if sat[0]>self.maxstage:
				self.maxstage=sat[0]
				print('The agent gain new stage at ',sat[0])
				return True
			#print('agent died while the length of the snake is ',sat[0])
		else:
			sat_=self.getSATuple(state_,self.getAction(state_))
			qnext=0
			if sat_ in self.qtable.keys():
				qnext=self.qtable[sat_]
			self.qtable[sat]+=self.learnRate*(reward+qnext-self.qtable[sat])
		return False

	def save(self,path):
		with open(path,'wb') as fp:
			n=len(self.qtable.keys())
			
			fp.write(struct.pack('=II',n,self.nSpaceSize))
			for stage,state,action in self.qtable.keys():
				value=self.qtable[(stage,state,action)]
				fp.write(struct.pack('=BIBd',stage,state,action,value))

	def open(self,path):
		self.qtable={}
		self.maxstage=0
		with open(path,'rb') as fp:
			buf=fp.read(8)
			n,self.nSpaceSize=struct.unpack('=II',buf)
			for i in range(n):
				buf=fp.read(14)
				stage,state,action,value=struct.unpack('=BIBd',buf)
				self.qtable[(stage,state,action)]=value



def main():
	env=PySnakeAI.Snake(SIZE)
	agent=QLearningAgent(SIZE)
	filename='qlearning.bin'
	if len(sys.argv)>1:
		filename=sys.argv[1]
		agent.open(filename)
	for i in range(300000):
		if agent.learn(env):
			print('round',i)
		if (i%1000)==999:
			print('save qtable at round',i)
			agent.save(filename)

if __name__=='__main__':
	main()
			