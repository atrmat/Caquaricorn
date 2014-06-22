#!/usr/bin/env python2
# Usage: ./JBc_Build_Information.py ../results/bbt.txt ../data/temp/temp_ ../results/JBc_Build_Information.dat
import sys

if (len(sys.argv) != 4):
	print "Example: ./JBc_Build_Information.py ../results/bbt.txt ../data/temp/temp_ ../results/JBc_Build_Information.dat"
else:
	inp = open(sys.argv[1], "r")
	oup = open(sys.argv[3], "w")
	lmin = 11
	lmax = 48
	titem = tmem = ttime = 0
	for i in range(1024):
		print "\r%d" %(i),
		sys.stdout.flush()
		info = inp.readline().split();
		mem = (int)(info[20][:-1])
		time = (int)(info[9][:-2]) + (int)(info[12][:-2]) + (int)(info[15][:-2])
		temp = open(sys.argv[2] + (str)(i) + ".txt", "r")
		item = 0
		for line in temp:
			[count, key] = line.split('\t')
			if (len(key) >= lmin and len(key) <= lmax):
				item += (int)(count)
		titem += item
		tmem += mem
		ttime += time
		oup.write("%d\t%.6f\t%d\n" %(i + 1, item * 1000000.0 / time, mem))
		oup.flush()
	print "\r1024"
	print "%.6f\t%d" % (titem * 1000000.0 / ttime, tmem / 1024)
