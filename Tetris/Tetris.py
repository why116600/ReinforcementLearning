import numpy as np
import random
import pygame
from pygame.locals import *

#俄罗斯方块的环境

#俄罗斯方块环境的宽和高
WIDTH=10
HEIGHT=20

TetrisItems=np.array([
	[#竖条条
		[
			[1,1,1,1],
			[0,0,0,0],
			[0,0,0,0],
			[0,0,0,0],
			],
		[
			[1,0,0,0],
			[1,0,0,0],
			[1,0,0,0],
			[1,0,0,0],
			],
		[
			[1,1,1,1],
			[0,0,0,0],
			[0,0,0,0],
			[0,0,0,0],
			],
		[
			[1,0,0,0],
			[1,0,0,0],
			[1,0,0,0],
			[1,0,0,0],
			],
		],
	[#方块
		[
			[1,1,0,0],
			[1,1,0,0],
			[0,0,0,0],
			[0,0,0,0],
			],
		[
			[1,1,0,0],
			[1,1,0,0],
			[0,0,0,0],
			[0,0,0,0],
			],
		[
			[1,1,0,0],
			[1,1,0,0],
			[0,0,0,0],
			[0,0,0,0],
			],
		[
			[1,1,0,0],
			[1,1,0,0],
			[0,0,0,0],
			[0,0,0,0],
			],
		],
	[#正L
		[
			[1,1,1,0],
			[1,0,0,0],
			[0,0,0,0],
			[0,0,0,0],
			],
		[
			[1,1,0,0],
			[0,1,0,0],
			[0,1,0,0],
			[0,0,0,0],
			],
		[
			[0,0,1,0],
			[1,1,1,0],
			[0,0,0,0],
			[0,0,0,0],
			],
		[
			[1,0,0,0],
			[1,0,0,0],
			[1,1,0,0],
			[0,0,0,0],
			],
		],
	[#反L
		[
			[1,1,1,0],
			[0,0,1,0],
			[0,0,0,0],
			[0,0,0,0],
			],
		[
			[0,1,0,0],
			[0,1,0,0],
			[1,1,0,0],
			[0,0,0,0],
			],
		[
			[1,0,0,0],
			[1,1,1,0],
			[0,0,0,0],
			[0,0,0,0],
			],
		[
			[1,1,0,0],
			[1,0,0,0],
			[1,0,0,0],
			[0,0,0,0],
			],
		],
	[#土
		[
			[0,1,0,0],
			[1,1,1,0],
			[0,0,0,0],
			[0,0,0,0],
			],
		[
			[1,0,0,0],
			[1,1,0,0],
			[1,0,0,0],
			[0,0,0,0],
			],
		[
			[1,1,1,0],
			[0,1,0,0],
			[0,0,0,0],
			[0,0,0,0],
			],
		[
			[0,1,0,0],
			[1,1,0,0],
			[0,1,0,0],
			[0,0,0,0],
			],
		],
	[#正扭
		[
			[1,1,0,0],
			[0,1,1,0],
			[0,0,0,0],
			[0,0,0,0],
			],
		[
			[0,1,0,0],
			[1,1,0,0],
			[1,0,0,0],
			[0,0,0,0],
			],
		[
			[1,1,0,0],
			[0,1,1,0],
			[0,0,0,0],
			[0,0,0,0],
			],
		[
			[0,1,0,0],
			[1,1,0,0],
			[1,0,0,0],
			[0,0,0,0],
			],
		],
	[#反扭
		[
			[0,1,1,0],
			[1,1,0,0],
			[0,0,0,0],
			[0,0,0,0],
			],
		[
			[1,0,0,0],
			[1,1,0,0],
			[0,1,0,0],
			[0,0,0,0],
			],
		[
			[0,1,1,0],
			[1,1,0,0],
			[0,0,0,0],
			[0,0,0,0],
			],
		[
			[1,0,0,0],
			[1,1,0,0],
			[0,1,0,0],
			[0,0,0,0],
			],
		],
	],dtype=np.int)

ALL_CUBIC=TetrisItems.shape[0]*TetrisItems.shape[1]
CUBIC_SIZE=4

INVALID_PUNISH=-1#无效操作时的惩罚
SURPLUS_PUNISH=-0.1#多余操作的惩罚

class Tetris:
	def __init__(self,limit=-1):
		self.nLimit=limit#最大回合数
		self.nAction=5
		self.Reset()


	def Reset(self):
		self.nCube=0#目前已经落下的方块数
		self.dead=False
		self.map=np.zeros((HEIGHT,WIDTH),dtype=np.int)
		self.NewCubic()
		return self.GetObservation()

	def NewCubic(self):
		self.xpos=WIDTH//2
		self.ypos=0
		nCheck=0
		while True:
			self.CubicIndex=random.randint(0,TetrisItems.shape[0]-1)
			self.rotation=random.randint(0,TetrisItems.shape[1]-1)
			self.cubic=TetrisItems[self.CubicIndex,self.rotation]
			if self.CheckCubic(self.xpos,self.ypos):
				break
			nCheck+=1
			if nCheck>ALL_CUBIC:
				return False
		self.nCube+=1
		return True

	def CheckCubic(self,xpos,ypos,cubic=None):
		cubic_map=np.zeros(self.map.shape,dtype=np.int)
		if type(cubic)==type(None):
			cubic=self.cubic
		h=cubic.shape[0]
		w=cubic.shape[1]
		if (ypos+h)>self.map.shape[0]:
			h=self.map.shape[0]-ypos
			if np.sum(cubic[h:,:])>0:#如果下面有部分被截断，则无效
				return False
		if (xpos+w)>self.map.shape[1]:
			w=self.map.shape[1]-xpos
			if np.sum(cubic[:,w:])>0:#如果右边有部分被截断，则无效
				return False

		cubic_map[ypos:ypos+h,xpos:xpos+w]=cubic[:h,:w]
		return np.sum(cubic_map&self.map)==0

	def GetObservation(self,doubleView=True):
		obs=self.map.copy()
		h=self.cubic.shape[0]
		w=self.cubic.shape[1]
		if (self.ypos+h)>self.map.shape[0]:
			h=self.map.shape[0]-self.ypos
		if (self.xpos+w)>self.map.shape[1]:
			w=self.map.shape[1]-self.xpos
		cubicmap=np.zeros(self.map.shape,dtype=np.int)
		cubicmap[self.ypos:self.ypos+h,self.xpos:self.xpos+w]=self.cubic[:h,:w]
		if doubleView:
			return np.array([self.map,cubicmap])
		obs=obs-cubicmap
		return obs

	def IsTerminated(self):
		return self.dead or self.nCube==self.nLimit

	def Step(self,action):
		if self.IsTerminated():
			return 0
		if action==1:#左移
			if self.xpos<=0:
				return INVALID_PUNISH
			xpos=self.xpos-1
			if not self.CheckCubic(xpos,self.ypos):
				return INVALID_PUNISH
			self.xpos=xpos
			return SURPLUS_PUNISH
		elif action==2:#右移
			if self.xpos>=(WIDTH-1):
				return INVALID_PUNISH
			xpos=self.xpos+1
			if not self.CheckCubic(xpos,self.ypos):
				return INVALID_PUNISH
			self.xpos=xpos
			return SURPLUS_PUNISH
		elif action==3:#向后翻转
			ro=(self.rotation+1)%TetrisItems.shape[1]
			cubic=TetrisItems[self.CubicIndex,ro]
			if not self.CheckCubic(self.xpos,self.ypos,cubic):
				return INVALID_PUNISH
			self.rotation=ro
			self.cubic=cubic
			return SURPLUS_PUNISH
		elif action==4:#向前翻转
			ro=self.rotation-1
			if ro<0:
				ro=TetrisItems.shape[1]-1
			cubic=TetrisItems[self.CubicIndex,ro]
			if not self.CheckCubic(self.xpos,self.ypos,cubic):
				return INVALID_PUNISH
			self.rotation=ro
			self.cubic=cubic
			return SURPLUS_PUNISH
		#如果都不是则向下掉
		ypos=self.ypos+1
		if self.CheckCubic(self.xpos,ypos):
			self.ypos=ypos
			return 0
		#如果无法往下掉，则说明方块碰到下面了，需要将方块融入地形中
		h=self.cubic.shape[0]
		w=self.cubic.shape[1]
		if (self.ypos+h)>self.map.shape[0]:
			h=self.map.shape[0]-self.ypos
		if (self.xpos+w)>self.map.shape[1]:
			w=self.map.shape[1]-self.xpos
		newmap=np.zeros(self.map.shape,dtype=np.int)
		newmap[self.ypos:self.ypos+h,self.xpos:self.xpos+w]=self.cubic[:h,:w]
		self.map=self.map|newmap
		reward=0
		#融合完成后判断是否存在已经填满一整行的地方
		for i in range(self.map.shape[0]):
			if np.sum(self.map[i,:])==self.map.shape[1]:#如果填满就消除一行，并增加奖励
				self.map[1:i+1]=self.map[0:i]
				if reward==0:
					reward=1
				else:
					reward+=reward
		#开始新的砖块
		if not self.NewCubic():
			self.dead=True
			return -self.map.shape[0]*self.map.shape[1]
		return reward

CUBE_SIZE=30

def main():
	pygame.init()
	green=pygame.Color(0,255,0)
	red=pygame.Color(255,0,0)
	white=pygame.Color(255,255,255)
	size=width,height=(CUBE_SIZE*WIDTH,CUBE_SIZE*HEIGHT)
	screen=pygame.display.set_mode(size)
	env=Tetris()
	bRunning=True
	while bRunning:
		for event in pygame.event.get():
			if event.type==pygame.QUIT:
				bRunning=False
				break
			if event.type==pygame.KEYDOWN:
				if event.key==K_LEFT:
					reward=env.Step(1)
					print('turning left got ',reward)
				elif event.key==K_RIGHT:
					reward=env.Step(2)
					print('turning right got ',reward)
				elif event.key==K_UP:
					reward=env.Step(3)
					print('turning around got ',reward)
				else:
					reward=env.Step(0)
					print('going down got ',reward)
				if env.IsTerminated():
					print('The end!')
		screen.fill(white)
		obs=env.GetObservation(False)
		for i in range(HEIGHT):
			for j in range(WIDTH):
				if obs[i,j]==1:
					screen.fill(green,rect=pygame.Rect(j*CUBE_SIZE,i*CUBE_SIZE,CUBE_SIZE,CUBE_SIZE))
				elif obs[i,j]==-1:
					screen.fill(red,rect=pygame.Rect(j*CUBE_SIZE,i*CUBE_SIZE,CUBE_SIZE,CUBE_SIZE))
		pygame.display.flip()
	pygame.quit()


if __name__=='__main__':
	main()