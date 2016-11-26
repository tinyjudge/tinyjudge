#!/usr/bin/env python2

import glob
import os

def name(x):
	for i in xrange(len(x) - 1, -1, -1):
		if x[i] == '.': return x[:i]
	return ''

def prob_number(x):
	for i in xrange(len(x) - 1, -1, -1):
		if not x[i].isdigit(): return (x[:i + 1], int(x[i + 1:]))
	return ('', int(x))


for d in glob.glob('prob/*'):
	d = d + '/'
	try:
		for i in glob.glob(d + '*.in'):
			nm = name(i)
			pn = prob_number(nm)
			if pn[0].endswith('/'): continue	# already processed
			try:
				os.rename(i, d + str(pn[1] - 1) + '.in')
			except KeyboardInterrupt:
				raise
			except:
				pass
			try:
				os.rename(nm + '.out', d + str(pn[1] - 1) + '.ans')
			except KeyboardInterrupt:
				raise
			except:
				pass
			try:
				os.rename(nm + '.ans', d + str(pn[1] - 1) + '.ans')
			except KeyboardInterrupt:
				raise
			except:
				pass
	except KeyboardInterrupt:
		raise
	except:
		import traceback
		print(traceback.format_exc())
