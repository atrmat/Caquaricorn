#!/usr/bin/env python2
# Usage: ./Merge.py input output 20000000 16

import sys

if (len(sys.argv) != 5):
	print "Example ./Merge.py input output 20000000 16";
else:
	inp = open(sys.argv[1], "r");
	oup = open(sys.argv[2], "w");
	ln = (int)(sys.argv[3]);
	step = (int)(sys.argv[4]);
	for i in range(step):
		s = [];
		line = inp.readline().split();
		l = len(line);
		for j in range(1, l):
			s.append((float)(line[j]));
		for j in range(1, ln / step):
			line = inp.readline().split();
			t = (int)(line[0]);
			for k in range(1, l):
				s[k - 1] += (float)(line[k]);
		oup.write("%d" % ((t + t / (i + 1) * i) / 2));
		for j in s:
			oup.write("\t%.6f" % (j / (ln / step)));
		oup.write("\n");
