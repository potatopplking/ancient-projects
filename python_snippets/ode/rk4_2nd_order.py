#!/usr/bin/env python2
# porovnani predani energie lehci a rychlejsi strely
# s pomalejsi a tezsi (stejna energie)
import pyqtgraph as pg
from numpy import arange
from PyQt4 import QtGui
from math import sin

def main():
	MAX_TIME = 0.1
	STEP_SIZE = 0.000001
	MAX_STEPS = int(MAX_TIME/STEP_SIZE)
	y = []
	yd = []
	# pocatecni podminky
	y.append(0.0)	# pocatecni pozice
	yd.append(392.23)	# pocatecni rychlost
	print 'pocitam...'
	i = 0
	m = 6.5e-3
	while i < MAX_STEPS:
		y.append(y[i]+STEP_SIZE*yd[i])
		k1 = prava_strana(y[i],yd[i],m)
		k2 = prava_strana(y[i]+STEP_SIZE/2*k1,yd[i],m)
		k3 = prava_strana(y[i]+STEP_SIZE/2*k2,yd[i],m)
		k4 = prava_strana(y[i]+STEP_SIZE*k3,yd[i],m)
		d = (k1+2*k2+2*k3+k4)/6.0
		yd.append(yd[i]+STEP_SIZE*d)
		i += 1
		#print(str(y[-1])+' '+str(yd[-1]))
	print('Last values: '+str(y[-1])+' '+str(yd[-1]))
	win = pg.GraphicsWindow(title="RK4 - 2nd order")
	win.resize(1000,600)
	win.setWindowTitle('RK4 - 2nd order')
	force_p = win.addPlot(title = 'Output')
	force_p.plot(arange(0,STEP_SIZE*(MAX_STEPS+1),STEP_SIZE),yd, pen = 'r')
	force_p.plot(arange(0,STEP_SIZE*(MAX_STEPS+1),STEP_SIZE),y, pen = 'g')
	QtGui.QApplication.instance().exec_()

def prava_strana(y,yd,m):
	# strela v prostredi
	k = 0.01
	return (-k/m*yd**2)

if __name__ == '__main__':
	main()