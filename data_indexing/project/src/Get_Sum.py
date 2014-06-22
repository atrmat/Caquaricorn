#!/usr/bin/python
import sys
filename = sys.argv[1]
ln = (int)(sys.argv[2])
fn = (int)(sys.argv[3])
cut = (int)(sys.argv[4])
file = open(filename)
s = 0
for i in range(ln):
	s += (int)(file.readline().split()[fn][:(-cut)])
print(s)

