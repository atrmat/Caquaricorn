#!/usr/bin/env python2
# Usage: ./JBc_Index_Information.py index_time.txt JBc_Index_Information.dat

import sys

if (len(sys.argv) != 3):
	print "Example: ./JBc_Index_Information.py ../results/index_time.txt ../results/JBc_Index_Information.dat"
else:
	inp = open(sys.argv[1], "r")
	oup = open(sys.argv[2], "w")
	last = 0
	for line in inp:
		if (len(line.split()) < 5):
			break
		oup.write("%d\t%.6f\n" %((int)(line.split()[1]), 1000 * 1000000.0 / ((int)(line.split()[4][:-2]) - last)))
		last = (int)(line.split()[4][:-2])
	print "%.6f" % (1024000 * 1000000.0 / last)
