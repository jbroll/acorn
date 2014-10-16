

class AcornModel {
  public:

    double	  z;
    AcornSurfGrp  surfaces;

    map<char *, AcornSurface *> names;		// Lookup surfaces by name.


    int		 nwave;
    double	*wavelength;
    double	*indicies;

    AcornModel() {
	
    };

    AcornModel& set(char *param, void *value) {
	SetVar...
    }

    AcornModel& setsurf(const char *surf, const char *param, void *value) {
	try {
	    if ( *surf == '\0') { s = current;
	    } else { 		  s = names.at(surf); }
	} catch ( const std::out_of_range& oor ) {
	    std::cerr << "Out of Range error: " << oor.what() << '\n';
	    
	}

	s->setparam(param, value);

	return this;
    }

    AcornSurfGrp *curgroup;			// Model construction management
    AcornSurface *currrent;

    AcornModel& appSurface(char *name, char *type) {
	
	curgroup.push_back(
	
    }

};

