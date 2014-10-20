
#include <stddef.h>
#include <string.h>

#include <cstdlib>
#include <iostream>
#include <fstream>

#include <string>
#include <vector>
#include <stack>

#include <map>

#include "../Acorn.hh"

#include "../AcornRay.hh"
#include "../AcornSurface.hh"




    std::map<const char*, AcornSurface *(*)(void)> AcornSurfaces = {
	{ "NONSEQCO", 	&AcornSurfGrpNonSeqConstructor	 } ,
    };

    static std::map<std::string, const char*> ZMXSurfaceMap = {
	{ "NONSEQCO",	 	"NONSEQCO"	}, 

	{ "standard",	 	"simple"	}, 
	{ "coordbrk",	 	"coordbrk"	}, 
	{ "evenasph",   	"evenasph"	}, 
	{ "dgrating",	 	"dgrating"	}, 
	{ "szernpha",	 	"zernike"	}, 
	{ "szernsag",	 	"zernike"	}, 

	{ "nsc_ssur",	 	"simple"	}, 
	{ "nsc_zsur",	 	"zernike"	}, 
	{ "nsc_annu",	 	"simple"	}, 

	{ "us_array.dll", 	"lens-array-rect"	}, 
	{ "us_hexarray.dll",	"lens-array-hex"	}
    };

    static std::map<std::string, std::map<int, std::string>> ZMXNSODMap = {
	{ "simple",  { 	{ 1, 	"R" } ,
			{ 2,	"K" } ,
			{ 3, 	"aper_max" } } 
	} ,
	{ "zernike", {	{  1,	"R" } ,
			{  2,	"K" } ,
			{  5,	"xdecenter" } ,
			{  6,	"ydecenter" } ,
			{ 15,	"nterms"    } }
	}
    };

    static std::map<std::string, std::map<int, std::string>> ZMXParmMap = {
	{ "lens-array-rect", { 	{ 1, "nx" } ,
			 	{ 2, "ny" } ,
			 	{ 3, "width" } ,
			 	{ 4, "height" } }
	} ,
	{ "coordbrk", { 	{ 1, "x" } ,
				{ 2, "y" } ,
				{ 3, "rx" } ,
				{ 4, "ry" } ,
				{ 5, "rz" } }
	} ,
	{ "evenasph", {		{ 1, "a2" } ,
				{ 2, "a4" } ,
				{ 3, "a6" } ,
				{ 4, "a8" } ,
				{ 5, "a10" } ,
				{ 6, "a12" } ,
				{ 7, "a14" } ,
				{ 8, "a16" } }
	} ,
	{ "dgrating", {		{ 1, "l/mm" } ,
				{ 2, "order" } }
	}
    };

class ZMX  { 

    static std::map<std::string, void (ZMX::*)(std::vector<char*>)> mtable;

  public:
    std::stack<AcornSurface *> surf_stack;


    AcornModel	 *model;
    AcornSurfGrp *surfaces;
    AcornSurface *current;

    int surftype;

 
    std::vector<char *> pup;
    std::vector<char *> mce;
    int mce_current;

    int Id;
    std::string Name;
    std::string Notes;
    std::string	comment;
    std::string nonseqid;
    std::string nonseq;
    std::string nsoexit;
    int debug;

    AcornSurfGrpType grouptype;

    int floating; double semi;
    double fieldx;
    double fieldy;

    AcornRay aray[1];


    AcornModel *Read(string filename) {
	model 	  = new AcornModel();
	grouptype = AcornSequential;

	surfaces = &model->surfaces;

	current = NULL;


	std::vector<char*> list = split(&cat(filename)[0], "\n");

        for ( auto &i : list ) {
	    std::vector<char*> line = split(i, " \t");

	    invoke(line[0], line);
        }

	return model;
    }

    void invoke(const char *method, std::vector<char*> argv) {
	if ( mtable.count(method) == 1 ) {
	    (this->*mtable[method])(argv);
	} else {
	     fprintf(stderr, "unknown method ZMX::%s\n", method);
	}
    }

    Keyword zmx_simple   (std::vector<char*> argv) {};
    Keyword zmx_coordbrk (std::vector<char*> argv) {};
    Keyword zmx_zernike  (std::vector<char*> argv) {};
    Keyword zmx_dgrating (std::vector<char*> argv) {};
    Keyword zmx_lens_array_rect (std::vector<char*> argv) {}
    Keyword zmx_evenasph (std::vector<char*> argv) {
	int eight = 8;

	//surf.setparam("nterms", 1, (void *) &eight);
    }

    Keyword ZVAN (std::vector<char*> argv) {}
    Keyword ZVCX (std::vector<char*> argv) {}
    Keyword ZVCY (std::vector<char*> argv) {}
    Keyword ZVDX (std::vector<char*> argv) {}
    Keyword ZVDY (std::vector<char*> argv) {}
    Keyword VERS (std::vector<char*> argv) {}
    Keyword UNIT (std::vector<char*> argv) {
	 // lens_unit src_prefix src_unit anal_prefix anal_unit args
    }
    Keyword ENVD (std::vector<char*> argv) {
	// temp pres args }	{ my set temperature $temp; my set presure $pres 
    }
    Keyword ENPD (std::vector<char*> argv) {
	//{ size args }		// my set pupilDiameter $size
    }
    Keyword GCAT (std::vector<char*> argv)	{}

    Keyword PRIM (std::vector<char*> argv) {}

    Keyword NAME (std::vector<char*> argv) { 
	//set     Name $args
    }
    Keyword NOTE (std::vector<char*> argv) {
	// lappend Notes $args
    }

    Keyword  SURF (std::vector<char*> argv) {
	Id 	= atoi(argv[1]);
	comment = "";
    }

    Keyword TYPE (std::vector<char*> argv) {

	const char *type = argv[1];
	const char *comm = argv[2];

	if ( !strcmp(type, "USERSURF") ) {
	    type = argv[2];
	    comm = argv[3];
	}


	if ( ZMXSurfaceMap.count(type) != 1 || AcornSurfaces.count(ZMXSurfaceMap[type]) != 1 ) {
	    fprintf(stderr, "zmx: unknown surface type : %s\n", type);
	    exit(1);
	} 

	surfaces->surfaces.push_back(AcornSurfaces[type]());
	current = model->surfaces.surfaces.back();

	current->setparam("comment", comm);

	if ( !strcmp(type, "NONSEQCO") ) {
	    surf_stack.push((AcornSurface *)&model->surfaces);
	    surfaces = (AcornSurfGrp *)(model->surfaces.surfaces.back());
	    return;
  	}
     }

    Keyword CURV (std::vector<char*> argv) {
	double curv = atof(argv[1]);

	if ( grouptype == AcornSequential && curv != 0.0 ) {
	    current->setparam("R", 1.0/curv);
	}
     }
     Keyword CONI (std::vector<char*> argv) { current->setparam("K",  atof(argv[1])); 	 }
     Keyword COMM (std::vector<char*> argv) { current->setparam("comment", argv[1]); }

    Keyword PARM (std::vector<char*> argv) {
	 int n		= atoi(argv[1]);
	 double	value	= atof(argv[2]);

	if ( grouptype == AcornSequential ) {
	    if ( ZMXParmMap.count(current->type) == 1 && ZMXParmMap[current->type].count(n) == 1 ) {
	        current->setparam(ZMXParmMap[current->type][n].c_str(), value);
	    }
	} else {
	     current->setparam("p$n", value);
	}
    }

    Keyword DISZ (std::vector<char*> argv) {
	 double thick 	= atof(argv[1]);

	if ( grouptype == AcornSequential ) {
	    current->thickness = thick;
	}
    }
    Keyword DIAM (std::vector<char*> argv) { 
	 double semi = atof(argv[1]);
	 
	// This is Zemax computed semi-diameter, not the aperture size. 
	//
	current->semi = semi;
    }
    Keyword SQOB (std::vector<char*> argv) { // aperture obscuration is true
     }
     Keyword ELOB (std::vector<char*> argv) { // aperture obscuration is true
     }

    Keyword SQAP (std::vector<char*> argv) {
	double w = atof(argv[1]);
	double h = atof(argv[2]);

	current->aper_min = w;
	current->aper_max = h;
     }

     Keyword ELAP (std::vector<char*> argv) {
	double w = atof(argv[1]);
	double h = atof(argv[2]);

	current->aper_type = "eliptical";

	current->aper_min = w/2.0;
	current->aper_max = h/2.0;
     }

     Keyword CLAP (std::vector<char*> argv) {
	double rad = atof(argv[2]);

	current->aper_type = "circular";
	current->aper_max  = rad;
     }
     Keyword FLAP (std::vector<char*> argv) {
	double rad = atof(argv[2]);

	current->aper_type = "circular";
	current->aper_max  = rad;
     }
     Keyword OBSC (std::vector<char*> argv) {
	double rad = atof(argv[2]);

	current->aper_type = "obstruction";
	current->aper_min = rad;
    }
    Keyword OBDC (std::vector<char*> argv) {
	double x = atof(argv[1]);
	double y = atof(argv[2]);

	current->aper_xoff = x;
	current->aper_xoff = y;
    }
    Keyword BLNK (std::vector<char*> argv) {}
    Keyword TRAC (std::vector<char*> argv) {}
    Keyword MOFF (std::vector<char*> argv) {}
    Keyword GLCZ (std::vector<char*> argv) {}
    Keyword RSCE (std::vector<char*> argv) {}
    Keyword RWRE (std::vector<char*> argv) {}

    Keyword NSCD (std::vector<char*> argv) {}
    Keyword NSOO (std::vector<char*> argv) {}
    Keyword NSOQ (std::vector<char*> argv) {}
    Keyword NSOS (std::vector<char*> argv) {}
    Keyword NSOU (std::vector<char*> argv) {}
    Keyword NSOV (std::vector<char*> argv) {}
    Keyword NSOW (std::vector<char*> argv) {}

    Keyword EFFL (std::vector<char*> argv) {}
    Keyword COAT (std::vector<char*> argv) {}
    Keyword COFN (std::vector<char*> argv) {}
    Keyword CONF (std::vector<char*> argv) {}
    Keyword DMFS (std::vector<char*> argv) {}
    Keyword FWGT (std::vector<char*> argv) {}
    Keyword FWGN (std::vector<char*> argv) {}
    Keyword GFAC (std::vector<char*> argv) {}
    Keyword GLRS (std::vector<char*> argv) {}
    Keyword GSTD (std::vector<char*> argv) {}
    Keyword HIDE (std::vector<char*> argv) {}
    Keyword MAZH (std::vector<char*> argv) {}
    Keyword MIRR (std::vector<char*> argv) {}
    Keyword MODE (std::vector<char*> argv) {}

    Keyword PFIL (std::vector<char*> argv) {}
    Keyword PICB (std::vector<char*> argv) {}
    Keyword POLS (std::vector<char*> argv) {}
    Keyword POPS (std::vector<char*> argv) {}
    Keyword PUSH (std::vector<char*> argv) {}
    Keyword PUPD (std::vector<char*> argv) {}
    Keyword PWAV (std::vector<char*> argv) {}
    Keyword RAIM (std::vector<char*> argv) {}
    Keyword ROPD (std::vector<char*> argv) {}
    Keyword SCOL (std::vector<char*> argv) {}
    Keyword SDMA (std::vector<char*> argv) {}
    Keyword SLAB (std::vector<char*> argv) {}
    Keyword TOL  (std::vector<char*> argv) {}
    Keyword TOLE (std::vector<char*> argv) {}
    Keyword VANN (std::vector<char*> argv) {}
    Keyword VCXN (std::vector<char*> argv) {}
    Keyword VCYN (std::vector<char*> argv) {}
    Keyword VDSZ (std::vector<char*> argv) {}
    Keyword VDXN (std::vector<char*> argv) {}
    Keyword VDYN (std::vector<char*> argv) {}
};

std::map<std::string, void (ZMX::*)(std::vector<char*>)> ZMX::mtable = {
#	include "zmx.mtable"
};


int main(int argc, char** argv) {

    ZMX zmx = ZMX();

    AcornModel *model = zmx.Read(argv[1]);

    return 0;
}

