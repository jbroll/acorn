#!/usr/bin/env python2.7
#

import os
import sys
import numpy as np

sys.path.append("/home/john/src/jbr.py")

import fits

for file in sys.argv[1:] :
    hdu = fits.hdu(file)
    med = float(np.median(hdu.data[1000:3000,1000:3000]))

    base = os.path.splitext(file)[0]
    print base, med

    fits.hdu(hdu.data/med).writeto(base + "+N.fits")

