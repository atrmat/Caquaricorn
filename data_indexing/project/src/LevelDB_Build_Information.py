#!/usr/bin/env python2
# Usage: ./LevelDB_Build_Information.py ../results/lbt.txt ../results/LBI.dat
import sys

if (len(sys.argv) != 3):
	print "Example: ./LevelDB_Build_Information.py ../results/lbt.txt ../results/LBI.dat";
else:
	inp = open(sys.argv[1], "r");
	oup = open(sys.argv[2], "w");
	lmin = 11;
	lmax = 48;
	titem = ttime = 0;
	for i in range(1024):
		print "\r%d" %(i),
		sys.stdout.flush();
		info = inp.readline().split();
		time = (int)(info[9][:-2]);
		ttime += time;
		item = (int)(info[5]);
		titem += item;
		oup.write("%d\t%.6f\n" %(i + 1, item * 1000000.0 / time));
		oup.flush();
	print "\r1024"
	print "%.6f" % (titem * 1000000.0 / ttime);
