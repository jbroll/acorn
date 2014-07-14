#!/usr/bin/env python2.7
#

import sys
import random

sys.path.append("/home/john/src/jbr.py")

import fits

image  = sys.argv[1]
cosmic = sys.argv[2]
count  = int(sys.argv[3])
scale  = float(sys.argv[4])

if len(sys.argv) > 5 :
    output = sys.argv[4]
else :
    output = image


img = fits.hdu(image)

print "shutgun: ", image, count, scale

for i in random.sample(range(0, 10000), count):
    ray = fits.hdu(cosmic + "/postage/cr" + "%04d" % i + ".fits")

    x = random.uniform(0, img.NAXIS1-12)
    y = random.uniform(0, img.NAXIS2-12)

    img.data[y:y+12, x:x+12] += ray.data*scale

img.writeto(output)


