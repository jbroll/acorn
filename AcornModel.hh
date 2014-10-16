

class AcornModel {
  public:

    double	  z;
    AcornSurfGrp  surfaces;

    std::map<const char *, AcornSurface *> names;		// Lookup surfaces by name.


    int		 nwave;
    double	*wavelength;
    double	*indicies;

    AcornModel() {
	
    };

    AcornModel& set(char *param, void *value) {
    	;
    }

    AcornSurfGrp *crgroup;			// Model construction management
    AcornSurface *current;

    template <type T>
    AcornModel& setsurf(const char *surf, const char *param, T value) {
	AcornSurface *s;

	if ( *surf == '\0') { s = current;
	} else {
	    if ( names.count(surf) != 1 ) {
		fprintf(stderr, "No surface of type : %s\n", surf);
	    }
	    s = names.at(surf);
	}

	s->setparam(param, value);

	return this;
    }

    AcornModel& appSurface(char *name, char *type) {
	AcornSurface *surf = AcornSurfaceConstructor(type);

	surf->name = name;

	current = crgroup->append(surf);
    }

};

