#!/usr/bin/env python2

import pyqtgraph as pg
from numpy import arange
from PyQt4 import QtGui
from math import sin

def main():
	MAX_X = 100.0
	STEP_SIZE = 0.001
	MAX_STEPS = int(MAX_X/STEP_SIZE)
	pp = []
	pp.append(3.141592654/3)	# 0. derivace
	pp.append(0.0)				# 1. derivace

	y = []
	temp = pp

	i = 0
	y.append(temp[0])
	while i < MAX_STEPS:
		j = 0
		while j < len(temp)-1:
			temp[j] += STEP_SIZE*temp[j+1]
			j += 1
		y.append(temp[0])
		t = temp[0]
		k1 = prava_strana(temp)
		temp[0] = t+STEP_SIZE/2*k1
		k2 = prava_strana(temp)
		temp[0] = t+STEP_SIZE/2*k2
		k3 = prava_strana(temp)
		temp[0] = t+STEP_SIZE*k3
		k4 = prava_strana(temp)
		temp[0] = t
		d = (k1+2*k2+2*k3+k4)/6.0
		temp[j] += STEP_SIZE*d
		i += 1

	win = pg.GraphicsWindow(title="RK4 - 2nd order")
	win.resize(1000,600)
	win.setWindowTitle('RK4 - 2nd order')
	force_p = win.addPlot(title = 'Output')
	force_p.plot(arange(0,STEP_SIZE*(MAX_STEPS+1),STEP_SIZE),y, pen = 'r')
	QtGui.QApplication.instance().exec_()

def prava_strana(y):
	# tlumene kyvadlo
	l = 1.0
	g = 9.81
	k = 0.1
	return (-g/l*sin(y[0])-k*y[1])

if __name__ == '__main__':
	main()