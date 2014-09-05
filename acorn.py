
import sys

from cffi import FFI
cffi = FFI()

cffi.cdef("""
	typedef double Vector3d[3];

	typedef int (*TraceFunc)(struct _MData *m, struct _Surface *s, struct _Ray *r);


	typedef struct _Ray {
	    Vector3d	p;
	    Vector3d	k;
	    int		wave;
	    int		vignetted;
	    float	intensity;
	} Ray;

	typedef struct _MData {
	    double z;
	    double *indicies;
	    double *wavelength;
	} MData;

	typedef struct _Surface {
	    char*	name;
	    char*	type;
	    char*	comment;
	    TraceFunc   traverse;

	    char*	aper_type;
	    char*	aper_param;

	    char*	aperture;
	    long	aper_data;
	    long	aper_leng;
	    
	    double	p[256];
	    char*	s[32];

	    char*	glass;
	    void*	glass_ptr;
	    double*	indicies;

	    long	enable;
	    long	annote;
	} Surface;

	TraceFunc traverse;
	int (*trace)(Surface *s, int nsurf);

    """)

Simple  = cffi.dlopen("surfaces/lib/Darwin.x86_64/simple.so") 
Zerkike = cffi.dlopen("surfaces/lib/Darwin.x86_64/zernike.so") 

# CFFI Proxy to allow set/get of a char* to/from a python string
#
class CFFIProxy(object) :
    def __init__(self, data, indx):
	self._data    = data
	self._indx    = indx
	self._length = 1

	self.__initialized = 1

    def __getattr__(self, attr):
	reply = getattr(self._data[self._indx], attr)

	if isinstance(reply, cffi.CData):
	    try:
		reply = cffi.string(reply)
	    except:
		pass

        return reply

    def __setattr__(self, attr, value):
	if ( not self.__dict__.has_key("_CFFIProxy__initialized") \
	  or self.__dict__.has_key(attr) ) :
	    self.__dict__[attr] = value
	    return

	if type(value) == str:
	    value = cffi.new("char []", value)

	setattr(self._data[self._indx], attr, value)


# Typedef adds a resize method and sets up the CFFIProxy for strings
#
class Typedef(object) :
    def __init__(self, typedef, *args):
	if len(args) == 0:
	    self._data   = cffi.new(typedef + "*")
	    self._length = 1
	else:
	    self._data   = cffi.new(typedef + "[]", args[0])
	    self._length = args[0]

	self.__initialized = 1

    def resize(self, newlen):
	newdata = cffi.new(cffi.typeof(self._data), newlen)

	newdata[0:min(len(self._data), newlen)] = self.data[0:min(len(self._data), newlen)]

	self._data = newdata

    def __getattr__(self, attr):
	reply = getattr(self._data, attr)

	if isinstance(reply, cffi.CData):
	    try:
		reply = cffi.string(reply)
	    except:
		pass

        return reply

    def __setattr__(self, attr, value):
	if ( not self.__dict__.has_key("_Typedef__initialized") \
	  or self.__dict__.has_key(attr) ) :
	    self.__dict__[attr] = value
	    return

	if type(value) == str:
	    value = cffi.new("char []", value)

	setattr(self._data, attr, value)

    def __getitem__(self, indx):
	return CFFIProxy(self._data, indx)


# Each of our data structures will have a constructor to allow
# us to add methods that will be implimented in C.
#
class Surface(Typedef):
    def __init__(self, *args): Typedef.__init__(self, "Surface", *args)


class Ray(Typedef):
    def __init__(self, *args): Typedef.__init__(self, "Ray", *args)

    def trace1(self, surf):
	print surf._data
	print surf._length

	acorn.trace1(self._data, self._length, surf._data, surf._length)


s1 = Surface(1)
s2 = Surface(1)

def cat(filename) :
    file = open('newfile.txt', 'r')
    data = file.read()
    close(file)

    return data

def NewSurfaceType(name):
    def __init__(self, *args): Typedef.__init__(self, name, *args)
    def traverse(self, rays):  self.so.traverse(rays)


    cffi.cdef(cat(name + ".h"))
    so = cffi.dlopen(name + ".so")

    cls = type("Surface" + name.title(), [Typedef], {
	  so: so

	, __init__: __init__
	, traverse: traverse
    })

    Surfaces[name] = cls

    return cls
    

s3 = Surface()

rays = Ray(100)
rays.trace1(s1[0])

x = 4;
#s3.traverse(x)

s3.aper_data = 4
s3.name = "ddd"
print s3.name
print s3.aper_data, s3.name


s1[0].name = "ccc"
print s1[0].name


sys.exit()

s1[0].aper_data = 2

s1[0].traverse = Simple.traverse

print s1[0].aper_data
s1.resize(2)
print s1[0].aper_data
print s1[1].aper_data

print s1[0].traverse

