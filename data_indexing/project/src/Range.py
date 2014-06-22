#!/usr/bin/env python2
# Usage: ./Range.py [file]

import sys;

if (len(sys.argv) != 2):
	print "Example: ./Range.py data_file_name.dat";
else:

	file = open(sys.argv[1], "r");
	first = True;
	ma = [];
	mi = [];
	for line in file:
		for i in range(len(line.split())):
			s = (float)(line.split()[i]);
			if (first):
				ma.append(s);
				mi.append(s);
			else:
				if (ma[i] < s):
					ma[i] = s;
				if (mi[i] > s):
					mi[i] = s;
		first = False;
	print "[%.2f,%.2f]" % (mi[0], ma[0]),;
	for i in range(1, len(mi)):
		print "\t[%.2f,%.2f]" % (mi[i], ma[i]),;
	print "";
