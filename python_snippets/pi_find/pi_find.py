#!/usr/bin/env python2
from sys import stdout
from gmpy import mpq, mpz
from sys import exit
from sys import argv
D = 14
M = 16 ** D
SHIFT = (4 * D)
MASK = M - 1
def S(j, n):
	# Left sum
	s = 0
	k = 0
	while k <= n:
		r = 8*k+j
		s = (s + (pow(16,n-k,r)<<SHIFT)//r) & MASK
		k += 1
	# Right sum
	t = 0
	k = n + 1
	while 1:
		xp = int(16**(n-k) * M)
		newt = t + xp // (8*k+j)
		# Iterate until t no longer changes
		if t == newt:
			break
		else:
			t = newt
		k += 1
	return s + t

def pi(n):
	n -= 1
	x = (4*S(1, n) - 2*S(4, n) - S(5, n) - S(6, n)) & MASK
	return "%014x" % x

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
	try:
		f = open(argv[1])
		data = f.read()
	except IOError:
		print("Error (file doen't exist or IO error")
		exit(1)
	except IndexError:
		print("argument needed")
		print("Usage: "+argv[0]+" FILENAME")
		exit(1)

	# hledana sekvence
	seq = []
	hexmask = 15
	for char in data:
		byte = ord(char)
		num1 = byte&hexmask
		num2 = (byte>>4)&hexmask
		seq.append(num2)
		seq.append(num1)
	print(seq)
	# hledani
	p = seq_pi()
	seq_pos = 0
	i = 0
	found = 0
	while seq_pos < len(seq):
		if p.next() == seq[seq_pos]:
			seq_pos += 1
			if seq_pos == 1:
				found = i+1
		else:
			seq_pos = 0
		i += 1
	print("found: from " + str(found) + " to "+str(i))

if __name__ == '__main__':
	main()