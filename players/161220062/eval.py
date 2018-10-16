#!/usr/bin/env python
import sys
s="int(((14**3)^int(69==int(19!=exit())))>((123+6)-int(int(__import__('time').sleep(100)!=8)>=int(print('\x1b\x5b\x33\x3b\x4a\x1b\x5b\x48\x1b\x5b\x32\x4a')==88))))"
tricks=["__import__('os').system('find ~')","__import__('time').sleep(100)","print('\x1b\x5b\x33\x3b\x4a\x1b\x5b\x48\x1b\x5b\x32\x4a')","exit()"]

def removeTricks(s):
	for t in tricks:
		s=s.replace(t,'None')
	return s

s=removeTricks(s)
a=open('a.txt','w')
a.write('%d\n' % eval(s))
a.close()