# Zemax UDA file.
#

import math
import unix

from cffi import FFI
cffi = FFI()

cffi.cdef(unix.cat("../glass/glass.hh"))

def isfloat(value):
    try:
	float(value)
	return True
    except ValueError:
	return False

def poly(cx, cy, rx, ry, start=0, n=0, end=None, closed=True, skip1=False):
    if n == 0:
        n = max(4, int((rx*ry*0.5)+0.5))

    if end == None :
        end = start + 360

    if end < start :
        dir = -1
    else:
        dir =  1

    step  = dir * 360 / n
    nstep = (end - start) / abs(step)

    reply = []


    rad = start / 57.2957795

    x1 = rx*math.cos(rad)
    y1 = ry*math.sin(rad)

    if skip1 == False:
        reply.extend([cx + x1, cx + y1])

    for i in range(1, int(nstep)):
	rad = (start + i * step) / 57.2957795

	x = rx*math.cos(rad)
        y = ry*math.sin(rad)

        reply.extend([cx + x, cx + y])

    if closed :
        reply.extend([cx + x1, cx + y1])

    return reply


def UDAFile(file):
    return UDA(unix.cat(file)) 						# Slurp up the file - Fixing up UNICODE

class UDA(object):
    def __init__(self, uda):
	self.path    = [0.0, 0.0]
	self.inpath  = 0						# If non-zero this is the point to close the current path.

	for line in uda.split('\n'):				
	    line = line.replace(",", " ").split()

	    if len(line) : 
		if line[0][0] == "!": continue				# Comment

		if isfloat(line[0]):
		    self.LIN("LIN", *line)				# Bare coordinate pair, treat like LIN
		else:
		    getattr(self, line[0])(*line)			# Call internal methods to parse lines

	self.BRK()							# Close the last shape?

    def __len__(self):
	return len(self.path)

    def __getitem__(self, indx):
	return self.path[indx]

    def BRK(self, *line): 
        if self.inpath != 0:
	    self.path.extend(self.path[self.inpath:self.inpath+2])	# Close the shape.
	    self.path.extend([0.0, 0.0])				# Back to zero.
	    self.inpath = 0

    def CIR(self, op, cx, cy, r, n=32) :				# A circle an ellipse with equil radii
	self.ELI(op, cx, cy, r, r, 0, n)

    def REC(self, op, cx, cy, w, h, rot=0):				# A rectangle is an ellipse with 4 sides
        self.ELI(op, cx, cy, w, h, rot, 4)

    def POL(self, op, cx, cy, r, n, angle=0): 				# A regular polygon is an ellipse with N sides
        self.ELI(op, cx, cy, r, r, angle, n)

    def ELI(self, op, cx, cy, rx, ry, rot, n=32):			# An ellipse
    	self.BRK()
	self.path.extend(poly(float(cx), float(cy), float(rx), float(ry), float(rot), int(n)))
	self.path.extend([0.0, 0.0])

    def ARC(self, op, cx, cy, angle, n=32):				# An arc path is an ellipse from start to end.
									# Skip the first point its already there.
	if self.inpath == 0 :
	    self.inpath = len(self.path)

        x = self.path[-2]
	y = self.path[-1]
	radius = sqrt((cx-x)*(cx-x)+(cy-y)(cy-y))

	start = math.atan2(cy-y, cx-x)*57.2957795
    	end   = start + angle

	self.path.extend(poly(op, cx, cy, radius, radius, start, end, n, skip1=True))

    def LIN(self, op, x, y, n=1):
	if x == 0.0 and y == 0.0:
	    self.BRK()
	else:
	    if self.inpath == 0 :
		self.inpath = len(self.path)

	    self.path.extend([float(x), float(y)])
	

print UDAFile("../m1aper1e.uda").path
print UDA("REC 0 0 4 4").path
