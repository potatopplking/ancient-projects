#!/usr/bin/env python2
# password generator
from sys import argv
from random import randint,uniform

def main():
	# special chars list
	special_chars = ('$','&','-','+','_')
	# default password length; can be specified by first argument
	def_pass_len = 8
	# default probability values; probabilities must sum to 1
	def_probs = []
	# lowecase; ascii decimal 97-122
	def_probs.append(0.4)
	# uppercase; ascii decimal 65-90
	def_probs.append(0.2)
	# numbers; ascii decimal 48-57
	def_probs.append(0.3)
	# special chars; specified manually
	def_probs.append(0.1)
	# try to get password length
	try:
		pass_len = int(argv[1])
	except IndexError:
		print("Password length not specified, using default value")
		pass_len = def_pass_len
	# generate password
	password = []
	for i in range(0,pass_len):
		rand_num = uniform(0.0,1.0)
		j = -1
		while rand_num > 0.001:
			j = j+1
			rand_num = rand_num - def_probs[j]
		# lowecase
		if j == 0:
			password.append(chr(randint(97,122)))				
		# uppercase
		elif j == 1:
			password.append(chr(randint(65,90)))
		# numbers
		elif j == 2:
			password.append(chr(randint(48,57)))
		# special symbols
		elif j == 3:
			password.append(special_chars[randint(0,len(special_chars)-1)])
	print(''.join(password))
			

if __name__ == '__main__':
	main()		
