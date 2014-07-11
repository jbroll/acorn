# Zemax AGF file.
#
# 
import glob
import unix

from cffi import FFI
cffi = FFI()

cffi.cdef(unix.cat("../glass/glass.hh"))

class AttrDict(dict):
    def __init__(self, *args, **kwargs):
	super(AttrDict, self).__init__(*args, **kwargs)
	self.__dict__ = self

class AGF(object):

    def __init__(self, file):
	self.current = None
	self.glass   = []

	for line in unix.cat(file).split('\n'):				# Slurp up the file - Fixing up UNICODE
	    line = line.split()

	    if len(line) : getattr(self, line[0])(*line)		# Call internal methods to parse lines


    def __len__(self):
	return len(self.glass)

    def __getitem__(self, indx):
	return self.glass[indx]

    def CC(self, *line):
	pass

    def NM(self, op, name, formula, MIL, Nd, Vd, exclude, status, *line):
	if self.current :
	    self.glass.append(self.current)

	self.current = AttrDict()
	self.current.update({ "name": name, "formula": formula, "MIL": MIL, "Nd": Nd, "Vd": Vd, "exclude": exclude, "status": status })

    def GC(self, op, *line):
	self.current["comment"] = " ".join(line)

    def ED(self, op, TCE, TCE100300, density, dPgF, ignthermal, *line):
	self.current.update({ "TCE": TCE, "TCE100300": TCE100300, "density": density, "dPgF": dPgF, "ignthermal": ignthermal })

    def CD(self, op, name, *line):
	self.current["c"] = [float(x) for x in line]

    def TD(self, op, D0, D1, D2, E0, E1, Ltk, temp, *line):
	self.current.update({ "D0": D0, "D1": D1, "D2": D2, "E0": E0, "E1": E1, "Ltk": Ltk, "temp": temp })

    def LD(self, op, name, *line): pass
    def OD(self, op, name, *line): pass
    def IT(self, op, name, *line): pass
    def ID(self, op, name, *line): pass

def GlassLoader(*paths):
    glassfiles = []

    for path in paths:
        for file in glob.glob(path + "/*.agf"):
	    glassfiles.append(AGF(file))

    return glassfiles

def GlassIndex(glassfiles):
    glassindex = {}

    for glassfile in glassfiles:
        for glass in glassfile:
    	    glassindex[glass.name] = glass

    return glassindex

GlassFiles = GlassLoader("../glass")
GlassIndex = GlassIndex(GlassFiles)

