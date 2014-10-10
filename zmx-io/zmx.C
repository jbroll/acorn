
#include <stddef.h>
#include <string.h>

#include <cstdlib>
#include <iostream>
#include <fstream>

#include <string>
#include <vector>
#include <stack>
#include <map>


#define Keyword void
#define Param

#define Type_int       1
#define Type_string    2
#define Type_double    3

typedef struct _VarMap {
    int type;
    int offset;
} VarMap;




typedef std::string string;



std::vector<char*> split(char *str, const char* delim) {
    std::vector<char*> list;

    char *here = strtok(str, delim);

    list.push_back(here);

    while ( (here = strtok(NULL, delim)) != NULL ) {
	list.push_back(here);
    }

    return list;
}

std::vector<char> cat(const std::string& filename) 
{
    int  	ch;
    bool	skip = 0;

    std::vector<char> reply;

    int ch0, ch1;

    std::ifstream inp(filename.c_str(), std::ios::in | std::ios_base::binary);


    if ( (ch0 = inp.get()) != -1 && (ch0 == 0xFE || ch0 == 0xFF) ) {
	if ( (ch1 = inp.get()) != -1  && ((ch0 == 0xFF && ch1 == 0xFE) || (ch0 == 0xFE && ch1 == 0xFF)) ) {
	    skip = 1;


	    if ( ch0 == 0xFE && ch1 == 0xFF ) {
		inp.get();
	    }
	} else {
	    reply.push_back(ch0);
	    reply.push_back(ch1);
	}
    } else {
	reply.push_back(ch0);
    }

    while ( (ch = inp.get()) != -1  && !inp.eof() ) {
	if ( skip ) inp.get();

	reply.push_back(ch);
    }

    return reply;
}

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


class AcornRay {
  public:
    int x;
    int y;
    int z;
};

enum AcornSurfGrpType { AcornSequential, AcornNonSequential };

struct AcornSurface {
	Param string type;
        std::map<const char *, VarMap> *vtable;

	int (*setparam)(void *, int set, const char* name, void *value);
	int (*traverse)(AcornRay *rays);

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

struct AcornSurfGrp {
	Param string type;
        std::map<const char *, VarMap> *vtable;

	int (*setparam)(void *, int set, const char* name, void *value);
	int (*traverse)(AcornRay *rays);


    AcornSurfGrpType seqtype;

    AcornSurfGrp () { 
	type    = "non-sequential";
	seqtype = AcornSequential;
    }
    AcornSurfGrp (AcornSurfGrpType Type) {
	seqtype = Type;
    }

    std::vector<AcornSurface *> surf;
};

static std::map<const char *, VarMap> vtable = {
#	include "zmx.vtable"
};

static int traverse(AcornRay *rays) { return 0; }

AcornSurfGrp *AcornSurfGrpConstructor(void) {
    AcornSurfGrp *surf = new AcornSurfGrp();
    surf->traverse = traverse;
    surf->vtable   = &vtable;

    return surf;
}



class AcornSurfaceCoordBrk : public AcornSurface {

  public:
    AcornSurfaceCoordBrk () { 
	type    = "coordbrk";
    }

    int setparam(int set, const char* name, void *value) { return 0; };
    int traverse(AcornRay *rays) { return 1; }
};

class AcornModel {
  public:

    AcornSurfGrp surfaces;
    int x;
};


AcornSurface *AcornSurfGrpNonSeqConstructor() {
    return (AcornSurface *) new AcornSurfGrp(AcornNonSequential);
}

AcornSurface *AcornSurfaceCoordBrkConstructor() {

    return (AcornSurface *) new AcornSurfaceCoordBrk();
}


    std::map<const char*, AcornSurface *(*)(void)> AcornSurfaces = {
	{ "NONSEQCO", 	&AcornSurfGrpNonSeqConstructor	 } ,
	{ "coordbrk", 	&AcornSurfaceCoordBrkConstructor }
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

    AcornSurfGrpType grouptype = AcornSequential;

    int floating; double semi;
    double fieldx;
    double fieldy;

    AcornRay aray[1];


    AcornModel *Read(string filename) {
	model = new AcornModel();

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

	//surf.setvar("nterms", 1, (void *) &eight);
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
	//{ size args }		{ my set pupilDiameter $size
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

	char *type = argv[1];
	char *comm = argv[2];

	if ( !strcmp(type, "USERSURF") ) {
	    type = argv[2];
	    comm = argv[3];
	}


	if ( ZMXSurfaceMap.count(type) != 1 || AcornSurfaces.count(ZMXSurfaceMap[type]) != 1 ) {
	    fprintf(stderr, "zmx: unknown surface type : %s\n", type);
	    exit(1);
	} 

	surfaces->surf.push_back(AcornSurfaces[type]());
	current = model->surfaces.surf.back();

	current->setparam(this, 1, "comment", comm);

	if ( !strcmp(type, "NONSEQCO") ) {
	    surf_stack.push((AcornSurface *)&model->surfaces);
	    surfaces = (AcornSurfGrp *)(model->surfaces.surf.back());
	    return;
  	}
     }

    Keyword CURV (std::vector<char*> argv) {
	double curv = atof(argv[1]);

	if ( grouptype == AcornSequential && curv != 0.0 ) {
	    current->setvar("R", 1.0/curv);
	}
     }
     Keyword CONI (std::vector<char*> argv) { current->setvar("K",  atof(argv[1])); 	 }
     Keyword COMM (std::vector<char*> argv) { current->setvar("comment", argv[1]); }

     Keyword PARM (std::vector<char*> argv) {
	 int n		= atoi(argv[1]);
	 double	value	= atof(argv[2]);

	if ( grouptype == AcornSequential ) {
	    if ( ZMXParmMap.count(current->type) == 1 && ZMXParmMap[current->type].count(n) == 1 ) {
	        current->setvar(ZMXParmMap[current->type][n].c_str(), value);
	    }
	} else {
	     current->setvar("p$n", value);
	}
     }
};

std::map<std::string, void (ZMX::*)(std::vector<char*>)> ZMX::mtable = {
#	include "zmx.mtable"
};


int main(int argc, char** argv) {

    ZMX zmx = ZMX();

    AcornModel *model = zmx.Read(argv[1]);

    return 0;
}

