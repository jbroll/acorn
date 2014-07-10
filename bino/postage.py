#!/usr/bin/env python2.7
#

import os
import sys
import numpy as np

sys.path.append("/home/john/src/jbr.py")

import fits
import starbase


j = 0

for file in sys.argv[1:] :
    base = os.path.splitext(file)[0]

    img = fits.hdu(file)
    tab = starbase.Starbase(base + ".tab")

    for i in range(0, len(tab)) :
	print tab[i].x, tab[i].y

	x = int(tab[i].x)
	y = int(tab[i].y)


	if x-6 < 0 or x+6 > img.NAXIS1 	\
	or y-6 < 0 or y+6 > img.NAXIS2:
	    continue

        stamp = fits.hdu(np.fmax(0.0, img.data[y-6:y+6, x-6:x+6]))

	stamp.writeto("postage/cr" + "%04d" % j + ".fits")

	j = j + 1

	if j > 9999 : break

