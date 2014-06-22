#!/usr/bin/env python2
# Usage: ./JBc_Update_Information.py ../results/update_time.txt ../results/JBc_Update_Information.dat

import sys

if (len(sys.argv) != 3):
	print "Example: ./JBc_Update_Information.py ../results/update_time.txt ../results/JBc_Update_Information.dat"
else:
	inp = open(sys.argv[1], "r")
	oup = open(sys.argv[2], "w")
	ttime = 0
	for line in inp:
		if (len(line.split()) != 28):
			break
		t = (int)(line.split()[8][:-2]) + (int)(line.split()[16][:-2]) + (int)(line.split()[21][:-2])
		ttime += t
		oup.write("%d\t%.6f\n" %((int)(line.split()[2]) + 1, 10000 * 1000000.0 / t))
	print "%.6f" % (10240000 * 1000000.0 / ttime)
