
#include <map>
#include "AcornRay.hh"

void SetVar(int set, char *that, VarMap &v, const char *name, void *value) {

    if ( set ) {
	switch ( v.type ) {
	    case Type_int:	*((int *)((char*)that+v.offset)) = *((int *) value);				break;
	    case Type_string:	*((string *)((char*)that+v.offset)) = (char *) value;				break;
	    case Type_double:	*((double *)((char*)that+v.offset)) = *((double *) value);			break;
	    default : ;
	}
    } else {
	switch ( v.type ) {
	    case Type_int:	*((int *) value)          = *((int *)((char*)that+v.offset));			break;
	    case Type_string:	(*(const char **) value)  = (*((string *)((char*)that+v.offset))).c_str();	break;
	    case Type_double:	*((double *) value)       = *((double *)((char*)that+v.offset));		break;
	    default : ;
	}
    }
}



#define ACORN_SURFACE 								\
	Param string type;							\
	Param double semi;							\
	Param double thickness;							\
	Param string aper_type;							\
	Param double aper_min;							\
	Param double aper_max;							\
	Param double aper_xoff;							\
	Param double aper_yoff;							\
	Param int    annote;							\
	double	*indicies;							\
										\
        std::map<const char *, VarMap> *vtable;					\
										\
	int (*traverse)(struct AcornModel *m, AcornSurface *s, AcornRay &rays);	\
	int (*initials)(struct AcornModel *m, AcornSurface *s);			\

struct AcornSurface {
       ACORN_SURFACE

	void setvar(const char *name, void *value) {
	    if ( vtable->count(name) == 1 ) {
		SetVar(1, (char *)this, (*vtable)[name], name, value);
	    } else {
		fprintf(stderr, "No param %s\n", name);
	    }
	}

	void setvar(const char *name, double value) { setvar(name, (void *) &value); }
	void setvar(const char *name, int    value) { setvar(name, (void *) &value); }
	void setvar(const char *name, string value) { setvar(name,           value); }
};


