
void SetVar(char *that, const char *name, int from_type, std::map<const char *, VarMap> *vtable, void *value) {

    if ( vtable->count(name) != 1 ) {
	fprintf(stderr, "acorn: no such parameter : %s\n", name);
	return;
    }

    VarMap v = vtable->at(name);

    switch ( v.type + from_type * 16 ) {
     case Type_int    + Type_double * 16 : *((int    *)((char*)that+v.offset)) = *((double *) value);			break;
     case Type_double + Type_double * 16 : *((double *)((char*)that+v.offset)) = *((double *) value);			break;
     case Type_string + Type_double * 16 : *((string *)((char*)that+v.offset)) = std::to_string(*((double *) value));	break;

     case Type_int    + Type_int    * 16 : *((int    *)((char*)that+v.offset)) = *((int *) value);			break;
     case Type_double + Type_int    * 16 : *((double *)((char*)that+v.offset)) = *((int *) value);			break;
     case Type_string + Type_int    * 16 : *((string *)((char*)that+v.offset)) = std::to_string(*((int *) value));	break;

     case Type_int    + Type_string * 16 : *((int    *)((char*)that+v.offset)) = std::stoi(*((string *) value));	break;
     case Type_double + Type_string * 16 : *((double *)((char*)that+v.offset)) = std::stod(*((string *) value));	break;
     case Type_string + Type_string * 16 : *((string *)((char*)that+v.offset)) = (*((string *) value));			break;
    }
}


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
	double	*indicies;							\
										\
        std::map<const char *, VarMap> *vtable;					\
										\
	int (*traverse)(struct AcornModel *m, AcornSurface *s, AcornRay &rays);	\
	int (*initials)(struct AcornModel *m, AcornSurface *s);			\

struct AcornSurface {
	ACORN_SURFACE

	AcornSurface* setparam(const char *name, double value) { SetVar((char *) this, name, Type_double, vtable, (void *) &value); return this; }
	AcornSurface* setparam(const char *name, int    value) { SetVar((char *) this, name, Type_int   , vtable, (void *) &value); return this; }
	AcornSurface* setparam(const char *name, string value) { SetVar((char *) this, name, Type_string, vtable, (void *) &value); return this; }
};


AcornSurface *AcornSurfaceConstructor(char *type);

