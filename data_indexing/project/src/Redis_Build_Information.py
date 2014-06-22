#!/usr/bin/env python2
# Usage: ./Redis_Build_Information.py ../results/rbt.txt ../results/Redis_Build_Information.dat

import sys
if (len(sys.argv) != 3):
	print "Example: ./Redis_Build_Information.py ../results/rbt.txt ../results/RBI.dat";
else:
	inp = open(sys.argv[1], "r");
	oup = open(sys.argv[2], "w");
	ttime = 0;
	smem = 0;
	for line in inp:
		(cnt, ti, lmem) = ((int)(line.split()[0]), (int)(line.split()[2][:-2]), (int)(line.split()[3][:-1]));
		if (cnt == 100):
			smem = lmem;
		ttime += ti;
		oup.write("%d\t%.6f\t%d\n" %(cnt, 7812500 * 1000000.0 / ti, lmem));
		smem = lmem;
		oup.flush();
	print "%.6f\t%d" % (2000000000 * 1000000.0 / ttime, smem);
