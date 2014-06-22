#!/usr/bin/env python2
# Usage: ./Redis_Update_Information.py ../results/rut.txt ../results/RUI.dat

import sys

if (len(sys.argv) != 3):
	print "Example: ./Redis_Update_Information.py ../results/rut.txt ../results/RUI.dat"
else:
	inp = open(sys.argv[1], "r")
	oup = open(sys.argv[2], "w")
	ttime = 0
	for line in inp:
		ti = (int)(line.split()[1][:-2])
		oup.write("%d\t%.6f\n" %((int)(line.split()[0]), 100 * 1000000.0 / ti))
		ttime += ti
	print "%.6f" % (1000000 * 1000000.0 / ttime)
