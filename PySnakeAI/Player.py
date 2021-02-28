import PySnakeAI
import pygame
from pygame.locals import *
import sys
import numpy as np

CUBE_SIZE=30
SIZE=3

def main():
	if len(sys.argv)>1:
		SIZE=int(sys.argv[1])
	pygame.init()
	green=pygame.Color(0,255,0)
	red=pygame.Color(255,0,0)
	white=pygame.Color(255,255,255)
	size=width,height=(CUBE_SIZE*SIZE,CUBE_SIZE*SIZE)
	screen=pygame.display.set_mode(size)
	env=PySnakeAI.Snake(SIZE)
	bRunning=True
	while bRunning:
		for event in pygame.event.get():
			if event.type==pygame.QUIT:
				bRunning=False
				break
			if event.type==pygame.KEYDOWN:
				if event.key==K_LEFT:
					reward=env.Step(3)
					print('going left got ',reward)
				elif event.key==K_RIGHT:
					reward=env.Step(1)
					print('going right got ',reward)
				elif event.key==K_UP:
					reward=env.Step(0)
					print('going up got ',reward)
				elif event.key==K_DOWN:
					reward=env.Step(2)
					print('going down got ',reward)
				if env.IsTerminated():
					print('The end!')
		screen.fill(white)
		obs=np.copy(env.map)
		for i in range(SIZE):
			for j in range(SIZE):
				if obs[i,j]>0:
					screen.fill(green,rect=pygame.Rect(j*CUBE_SIZE,i*CUBE_SIZE,CUBE_SIZE,CUBE_SIZE))
				elif obs[i,j]==-1:
					screen.fill(red,rect=pygame.Rect(j*CUBE_SIZE,i*CUBE_SIZE,CUBE_SIZE,CUBE_SIZE))
		pygame.display.flip()
	pygame.quit()


if __name__=='__main__':
	main()
