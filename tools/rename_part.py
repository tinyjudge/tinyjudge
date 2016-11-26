#!/usr/bin/env python2

import glob
import os

for d in glob.glob('part/*'):
	d = d + '/'
	try:
		for i in glob.glob(d + '*'):
			for j in glob.glob(i + '/*'):
				os.rename(j, i + '/../' + j[j.rfind('/') + 1:])
			os.rmdir(i)
	except KeyboardInterrupt:
		raise
	except:
		import traceback
		print(traceback.format_exc())
