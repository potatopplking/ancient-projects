#!/usr/bin/env python2

import pyqtgraph as pg
from numpy import arange
from PyQt4 import QtGui
from math import sin

def main():
	MAX_STEPS = 20000
	STEP_SIZE = 0.001
	y = []
	yd = []

	# pocatecni podminky
	y.append(3.141592654/3) # pocatecni pozice kyvadlo
	yd.append(0)			# pocatecni rychlost kyvadla
	print 'pocitam...'
	i = 0
	while i < MAX_STEPS:
		y.append(y[i]+STEP_SIZE*yd[i])
		dd = prava_strana(y[i],yd[i])
		yd.append(yd[i]+STEP_SIZE*dd)
		i += 1
	win = pg.GraphicsWindow(title="Euler's method test - 2nd order")
	win.resize(1000,600)
	win.setWindowTitle('Euler\'s method test - 2nd order')
	force_p = win.addPlot(title = 'Output')
	force_p.plot(arange(0,STEP_SIZE*(MAX_STEPS+1),STEP_SIZE),y, pen = 'r')
	QtGui.QApplication.instance().exec_()

def prava_strana(y,yd):
	# testovaci fce: return ((-2*yd-y)/3.0)
	# kyvadlo
	l = 1.0
	g = 9.81
	return (-g/l*sin(y))

if __name__ == '__main__':
	main()