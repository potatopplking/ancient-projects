#!/usr/bin/env python2

import pyqtgraph as pg
from numpy import arange
from PyQt4 import QtGui

def main():
	y = []
	STEP_SIZE = 0.0001
	MAX_STEPS = 100000
	# pocatecni podminka: y(0) = 5
	y.append(5)
	# rovnice: y' + 3y = 2
	print 'pocitam...'
	i = 0
	while i < MAX_STEPS:
		k1 = prava_strana(y[i])
		k2 = prava_strana(y[i]+STEP_SIZE/2*k1)
		k3 = prava_strana(y[i]+STEP_SIZE/2*k2)
		k4 = prava_strana(y[i]+STEP_SIZE*k3)
		y.append(y[i]+STEP_SIZE/6*(k1+2*k2+2*k3+k4))
		i += 1

	win = pg.GraphicsWindow(title="RK4 test")
	win.resize(1000,600)
	win.setWindowTitle('RK4 test')
	force_p = win.addPlot(title = 'Output')
	force_p.plot(arange(0,STEP_SIZE*(MAX_STEPS+1),STEP_SIZE),y, pen = 'r')
	QtGui.QApplication.instance().exec_()

def prava_strana(y):
	return (2-y)

if __name__ == '__main__':
	main()