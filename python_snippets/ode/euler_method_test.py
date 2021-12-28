#!/usr/bin/env python2

import pyqtgraph as pg
from numpy import arange
from PyQt4 import QtGui

def main():
	STEP_SIZE = 0.0001
	MAX_STEPS = 100000
	print 'Pocitam...'
	x = []
	# pocatecni podminky: x(0) = 5
	x.append(5.0)
	# rovnice: x' + 3x = 2
	# x' = 2 - 3x
	# x[n+1] = x[n] + x' * h = x[n] + (2-3x[n]) * h
	i = 0
	while i < MAX_STEPS:
		x.append(x[i]+prava_strana(x[i])*STEP_SIZE)
		i += 1
	win = pg.GraphicsWindow(title="Euler's method test")
	win.resize(1000,600)
	win.setWindowTitle('Euler\'s method test')
	force_p = win.addPlot(title = 'Output')
	force_p.plot(arange(0,STEP_SIZE*(MAX_STEPS+1),STEP_SIZE),x, pen = 'r')
	QtGui.QApplication.instance().exec_()

def prava_strana(y):
	return (2-3*y)

if __name__ == "__main__":
	main()