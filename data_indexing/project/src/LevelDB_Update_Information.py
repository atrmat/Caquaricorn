#!/usr/bin/env python2
# Usage: ./LevelDB_Update_Information.py ../results/lut.txt ../results/LevelDB_Update_Information.dat

import sys

if (len(sys.argv) != 3):
	print "Example: ./LevelDB_Update_Information.py ../results/lut.txt ../results/LevelDB_Update_Information.dat"
else:
	inp = open(sys.argv[1], "r")
	oup = open(sys.argv[2], "w")
	ttime = 0
	for line in inp:
		if (len(line.split()) != 2):
			break
		t = (int)(line.split()[1]);
		ttime += t
		oup.write("%d\t%.6f\n" %((int)(line.split()[0]) / 10000, 10000 * 1000000.0 / t))
	print "%.6f" % (10240000 * 1000000.0 / ttime)
