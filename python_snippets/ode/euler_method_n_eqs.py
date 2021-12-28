#!/usr/bin/env python2
from math import sin

def main():
	MAX_TIME = 100.0
	STEP_SIZE = 0.0001
	MAX_STEPS = MAX_TIME/STEP_SIZE
	pp = [1.0,0.0]
	y = []
	y.append(pp)
	i = 0
	x = 0
	print('pocitam')
	while i < MAX_STEPS-1:
		r = prava_strana(x,y[i])
		j = 0
		y.append([])
		while j < len(r):
			y[i+1].append(y[i][j]+STEP_SIZE*r[j])
			j += 1
		i += 1
		x += STEP_SIZE

	print('zapisuji')
	f = open('output','w')
	x = 0.0
	for line in y:
		temp = str(x)
		for val in line:
			temp += ' '+str(val)
		temp += '\n'
		f.write(temp)
		x += STEP_SIZE
	f.close()

def prava_strana(x,y):
	# l = 1.0
	# g = 9.81
	# k = 0.1
	#return [y[1],-g/l*sin(y[0])-k*y[1]]
	return [y[1],-y[0]]


if __name__=='__main__':
	main()