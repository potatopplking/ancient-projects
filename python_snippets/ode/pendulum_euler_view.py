#!/usr/bin/env python2
import pygame
from math import sin
from math import cos
l = 3.0
def main():
	# ode init
	STEP_SIZE = 0.01 # svazano s fps, pozor na hodnoty
	y = [3.141592654/3,0.0] # PP
	x = 0
	# draw init
	black = ( 0, 0, 0)
	white = ( 255, 255, 255)
	green = ( 0, 255, 0)
	red = ( 255, 0, 0)
	x_size = 700
	y_size = 500
	pygame.init()
	size = [x_size,y_size]
	screen = pygame.display.set_mode(size)
	pygame.display.set_caption("Box viewer")
	done = False
	clock = pygame.time.Clock()
	while done == False:
		r = prava_strana(x,y)
		j = 0
		while j < len(r):
			y[j] = y[j]+STEP_SIZE*r[j]
			j += 1
		x += STEP_SIZE
		for event in pygame.event.get(): # User did something
			if event.type == pygame.QUIT: # If user clicked close
				done = True # Flag that we are done so we exit this loop
		screen.fill(black)
		pygame.draw.line(screen, red, (x_size/2, 0), (x_size/2+100*l*sin(y[0]),100*l*cos(y[0])))
		pygame.draw.circle(screen,white,(int(x_size/2+100*l*sin(y[0])),int(100*l*cos(y[0]))),5)
		pygame.display.flip()
		clock.tick(int(1/STEP_SIZE))
	pygame.quit()

def prava_strana(x,y):
	g = 9.81
	k = 0.1
	return [y[1],-g/l*sin(y[0])-k*y[1]]
	#return [y[1],-y[0]]


if __name__=='__main__':
	main()