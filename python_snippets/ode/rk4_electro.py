#!/usr/bin/env python2
from math import sin
from numpy import matrix
from numpy import diag

def main():
	MAX_TIME = 10000.0
	STEP_SIZE = 0.01
	MAX_STEPS = MAX_TIME/STEP_SIZE
	pp = [0.0,0.0] # Ua, Ub
	y = []
	y.append(pp)
	eq_num = len(pp)
	temp = [0.0]*eq_num
	i = 0
	x = 0
	print('pocitam')
	while i < MAX_STEPS-1:
		y.append([])
		k1 = prava_strana(x,y[i])
		j = 0
		while j < eq_num:
			temp[j] = (y[i][j]+STEP_SIZE/2*k1[j])
			j += 1
		k2 = prava_strana(x+STEP_SIZE/2,temp)
		j = 0
		while j < eq_num:
			temp[j] = (y[i][j]+STEP_SIZE/2*k2[j])
			j += 1
		k3 = prava_strana(x+STEP_SIZE/2,temp)
		j = 0
		while j < eq_num:
			temp[j] = (y[i][j]+STEP_SIZE*k3[j])
			j += 1
		k4 = prava_strana(x+STEP_SIZE,temp)
		j = 0
		while j < eq_num:
			y[i+1].append(y[i][j]+STEP_SIZE/6*(k1[j]+2*k2[j]+2*k3[j]+k4[j]))
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
	U0 = 2.0
	R = 2000.0
	Rz = 4000.0
	C = 1.0e-6
	# return [y[1],-y[0]]
	Ua = y[0]
	Ub = y[1]
	return [ -((Ua-U0)*1/R + (Ua-Ub)*1/Rz), -((Ub-Ua)*1/Rz + Ub*1/R)]



if __name__=='__main__':
	main()