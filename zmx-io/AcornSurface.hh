
#define ACORNSURFACE 								\
	Param string type;							\
	Param double semi;							\
	Param double thickness;							\
	Param string aper_type;							\
	Param double aper_min;							\
	Param double aper_max;							\
	Param double aper_xoff;							\
	Param double aper_yoff;							\
        std::map<const char *, VarMap> *vtable;					\
										\
	int (*traverse)(AcornRay *rays);

struct AcornSurface {
       ACORNSURFACE

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


