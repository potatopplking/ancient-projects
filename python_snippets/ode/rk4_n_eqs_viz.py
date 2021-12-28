#!/usr/bin/env python3

import pyqtgraph as pg
from numpy import arange, matrix, diag
from PyQt5 import QtGui
from math import sin,pi

def main():
	MAX_TIME = 1
	STEP_SIZE = 0.00001
	MAX_STEPS = MAX_TIME/STEP_SIZE
	# u_c, i
	pp = [0.0, 0.0]
	y = []
	uc = []
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
		uc.append(y[i+1][0])
		i += 1
		x += STEP_SIZE

	print(max(uc))
	win = pg.GraphicsWindow(title="RK4")
	win.resize(1000,600)
	win.setWindowTitle('RK4')
	force_p = win.addPlot(title = 'Output')
	force_p.plot(arange(0,STEP_SIZE*(MAX_STEPS-1),STEP_SIZE),uc, pen = 'r')
	QtGui.QApplication.instance().exec_()
	
def prava_strana(x,y):
	R = 10 # ohms
	L = 1 # H
	C = 15e-6 # F
	# SRC
	U0 = 5
	f = 41.1
	# u_c_dot, i_dot
	u_c = y[0]
	i = y[1]
	u_c_dot = i / C
	i_dot = 1/L * (U0*sin(2*pi*f*x) - i * R - u_c)
	return [u_c_dot, i_dot]



if __name__=='__main__':
	main()
