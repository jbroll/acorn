#ifndef AcornSurface_HH
#define AcornSurface_HH

#include <map>
#include "Acorn.hh"
#include "AcornGlass.hh"

typedef std::map<const char*, VarMap, cstrcmp> CStrVarMapMap;

void SetVar(char *that, const char *name, int from_type, CStrVarMapMap *vtable, void *value);

#define ACORN_SURFACE 								\
	Param string type;							\
	Param string name;							\
	Param double semi;							\
	Param double thickness;							\
	Param string aper_type;							\
	Param double aper_min;							\
	Param double aper_max;							\
	Param double aper_xoff;							\
	Param double aper_yoff;							\
	Param int    annote;							\
										\
	double	   *indicies;							\
	string	   *glass;							\
										\
        CStrVarMapMap *vtable;							\
										\
	int (*traverse)(struct AcornModel *m, AcornSurface *s, AcornRay &rays);	\
	int (*initials)(struct AcornModel *m, AcornSurface *s);			\

struct AcornSurface {
	ACORN_SURFACE

	AcornSurface* setparam(const char *name, int    value) { SetVar((char *) this, name, Type_int   , vtable, (void *) &value); return this; }
	AcornSurface* setparam(const char *name, double value) { SetVar((char *) this, name, Type_double, vtable, (void *) &value); return this; }
	AcornSurface* setparam(const char *name, string value) { SetVar((char *) this, name, Type_string, vtable, (void *) &value); return this; }
};


AcornSurface *AcornSurfaceConstructor(const char *type);

#endif
