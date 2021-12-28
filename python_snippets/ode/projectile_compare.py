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
	E1 = []
	E2 = []
	m1 = 6.5e-3
	m2 = 8e-3
	v10 = 392.23
	v20 = 353.55
	# pocatecni podminky
	y.append(0.0)	# pocatecni pozice
	yd.append(v10)	# pocatecni rychlost
	E1.append(V2E(v10,m1))
	m = m1
	print 'pocitam...'
	i = 0
	while i < MAX_STEPS:
		y.append(y[i]+STEP_SIZE*yd[i])
		k1 = prava_strana(y[i],yd[i],m)
		k2 = prava_strana(y[i]+STEP_SIZE/2*k1,yd[i],m)
		k3 = prava_strana(y[i]+STEP_SIZE/2*k2,yd[i],m)
		k4 = prava_strana(y[i]+STEP_SIZE*k3,yd[i],m)
		d = (k1+2*k2+2*k3+k4)/6.0
		yd.append(yd[i]+STEP_SIZE*d)
		E1.append(V2E(yd[-1],m))
		i += 1
		#print(str(y[-1])+' '+str(yd[-1]))
	print('Last values: '+str(y[-1])+' '+str(yd[-1]))
	y1d = yd;
	y = []
	yd = []
	# pocatecni podminky
	y.append(0.0)	# pocatecni pozice
	yd.append(v20)	# pocatecni rychlost
	E2.append(V2E(v20,m2))
	m = m2
	print 'pocitam...'
	i = 0
	while i < MAX_STEPS:
		y.append(y[i]+STEP_SIZE*yd[i])
		k1 = prava_strana(y[i],yd[i],m)
		k2 = prava_strana(y[i]+STEP_SIZE/2*k1,yd[i],m)
		k3 = prava_strana(y[i]+STEP_SIZE/2*k2,yd[i],m)
		k4 = prava_strana(y[i]+STEP_SIZE*k3,yd[i],m)
		d = (k1+2*k2+2*k3+k4)/6.0
		yd.append(yd[i]+STEP_SIZE*d)
		E2.append(V2E(yd[-1],m))
		i += 1
		#print(str(y[-1])+' '+str(yd[-1]))
	print('Last values: '+str(y[-1])+' '+str(yd[-1]))
	y2d = yd;
	
	# plotting results
	plt = pg.plot()
	plt.setWindowTitle('Loss of energy of a bullet due to drag force')
	plt.addLegend()
	plt.plot(arange(0,STEP_SIZE*(MAX_STEPS+1),STEP_SIZE),E1, pen = 'r', name = '6.5g, ~392 m/s')
	plt.plot(arange(0,STEP_SIZE*(MAX_STEPS+1),STEP_SIZE),E2, pen = 'g', name = '8g, ~354 m/s')
	plt.setLabel('left','Energy',units='J')
	plt.setLabel('bottom','Time',units='s')
	QtGui.QApplication.instance().exec_()

# velocity to energy
def V2E(v,m):
	return 0.5*m*v**2

def prava_strana(y,yd,m):
	# strela v prostredi
	k = 0.01
	try:
		return (-k/m*yd**2)
	except OverflowError:
		print('overflow!!')
		print(-k/m)
		print(yd)
		exit(1);

if __name__ == '__main__':
	main()