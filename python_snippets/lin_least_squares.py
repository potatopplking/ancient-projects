#!/usr/bin/env python2
# linear least squares - aproximace na linearni funkci (primku)
from re import compile
from numpy import mean
from numpy import std
def main():
	point_raw_data = \
'''
0	0.02
0.5	0.39
1	0.87
1.5	1.37
2	1.88
2.5	2.4
3	2.92
3.5	3.45
4	4
4.5	4.51
4.95	4.99
'''
	regex = compile('(-*\d*\.*\d\d*)\s\s*(-*\d*\.*\d\d*)')
	res = regex.findall(point_raw_data)
	# koeficienty rovnic vzniklych z derivovane funkce souctu kvadratu chyb
	# prvni rovnice: df/da
	# druha rovnice: df/db
	a1 = 0.0 # koef. a u prvni rovnice
	b1 = 0.0 # koef. b u prvni rovnice
	c1 = 0.0 # abs. clen u prvni rovnice
	a2 = 0.0 # koef. a u druhe rovnice
	b2 = 0.0 # koef. b u druhe rovnice
	c2 = 0.0 # abs. clen u druhe rovnice
	# promenne pro korelacni koeficient
	x_arr = []
	y_arr = []
	if res:
		for point in res:
			x = float(point[0])
			y = float(point[1])
			x_arr.append(x)
			y_arr.append(y)
			a1 += x**2
			b1 += x
			c1 += -y*x
			a2 += x
			b2 += 1
			c2 += -y
		# cramerovo pravidlo
		det = a1*b2 - a2*b1
		det_a = b1*c2 - c1*b2
		det_b = a2*c1 - a1*c2
		a = det_a/det
		b = det_b/det
		print(str(a) + 'x + ' + str(b))

		# vypocet korelacniho koeficientu
		x_mean = mean(x_arr)
		y_mean = mean(y_arr)
		# kovariance
		i = 0
		sxy = 0.0
		while i < len(x_arr):
			sxy += (x_arr[i]-x_mean)*(y_arr[i]-y_mean)
			i += 1
		sxy /= len(x_arr)
		# korelacni koeficient
		R = len(x_arr)/(len(x_arr)-1)*sxy/(std(x_arr)*std(y_arr))
		print('R = ' + str(R))
	else:
		print("chyba pri zpracovani dat")


if __name__ == '__main__':
	main()
