#!/usr/bin/env python2
from gmpy import mpq, mpz
import pyqtgraph as pg
from PyQt4 import QtGui

def mod1(x):
	return x-mpz(x)

def seq_pi():
	x = 0
	n = 1
	while 1:
		p = mpq((120*n-89)*n+16, (((512*n-1024)*n+712)*n-206)*n+21)
		x = mod1(16*x + p)
		n += 1
		yield int(16*x)

def main():
	p = seq_pi()
	i = 0
	MAX_ITER = 10000
	occurence = [0]*16;
	while i < MAX_ITER:
		occurence[p.next()] += 1;
		i += 1
	occ_float = []
	for val in occurence:
		occ_float.append(float(val)/MAX_ITER)
	win = pg.GraphicsWindow(title="Pi rozdeleni")
	win.resize(1000,600)
	win.setWindowTitle('Pi rozdeleni')
	force_p = win.addPlot(title = 'Pi rozdeleni')
	force_p.plot(occ_float, pen = 'r')
	QtGui.QApplication.instance().exec_()

if __name__ == '__main__':
	main()