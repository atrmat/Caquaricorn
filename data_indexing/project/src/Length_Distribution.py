#/usr/bin/env python2
# Usage: ./Length_Distribution.py ../results/lno.txt ../results/Length_Distribution.dat
import sys

if (len(sys.argv) != 3):
	print "Example: ./Length_Distribution.py ../results/lno.txt ../results/Length_Distribution.dat"
else:
	inp = open(sys.argv[1], "r")
	oup = open(sys.argv[2], "w")
	s = []
	sum = 0
	for line in inp:
		s.append([(int)(line.split()[0]), (int)(line.split()[1])])
		sum += (int)(line.split()[1])
	k = 0
	for data in s:
		oup.write("%d\t%.6f\n" %(data[0], (float)(data[1]) / sum))
		k += (float)(data[1]) / sum
	print k
