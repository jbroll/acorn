

class AcornModel {
  public:

    Param double  z;

    AcornSurfGrp  surfaces;

    std::map<const char *, AcornSurface *> names;		// Lookup surfaces by name.


    int		 nwave;
    double	*wavelength;
    double	*indicies;

    CStrVarMapMap *vtable;

    AcornModel() {
	z 		= 0.0;

	nwave		= 1;
	double	*fiveK  = (double *) malloc(sizeof(double));

	*fiveK = 5000.0;

	wavelength	= fiveK;
	indicies	= NULL;
    };

    AcornModel* setparam(const char *name, int    value) { SetVar((char *) this, name, Type_int   , vtable, (void *) &value); return this; }
    AcornModel* setparam(const char *name, double value) { SetVar((char *) this, name, Type_double, vtable, (void *) &value); return this; }
    AcornModel* setparam(const char *name, string value) { SetVar((char *) this, name, Type_string, vtable, (void *) &value); return this; }

    AcornSurfGrp *crgroup;			// Model construction management
    AcornSurface *current;


    template <typename T>
    AcornModel *setsurf(const char *surf, const char *param, T value) {
	AcornSurface *s;

	if ( surf == NULL || *surf == '\0') { s = current;
	} else {
	    if ( names.count(surf) != 1 ) {
		fprintf(stderr, "No surface of type : %s\n", surf);
	    }
	    s = names.at(surf);
	}

	s->setparam(param, value);

	return this;
    }

    AcornModel *appSurface(char *name, char *type, bool grp=false) {
	AcornSurface *surf = AcornSurfaceConstructor(type);

	surf->name = name;

	current = crgroup->append(surf);

//	if ( grp ) {
	    

	return this;
    }


    AcornModel *trace(AcornRay *rays, char *surfs=NULL, int threads=0, AcornRay *xray=NULL, size_t ray_size=sizeof(AcornRay)) {
    }

};

