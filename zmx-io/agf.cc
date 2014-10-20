//# Zemax AGF file.
//#
//# 
//# CC Updated '12.04.24
//# NM APL1 1 517696 1.517277 69.563209 0 2 0
//# GC 
//# ED 0.000000000E+000 0.000000000E+000 0.000000000E+000 0.000000000E+000 0 0
//# CD 2.276240300E+000 -9.687464700E-003 9.233037800E-003 3.894085300E-004 -3.801873300E-005 2.069322500E-006 0.000000000E+000 0.000000000E+000 0.000000000E+000 0.000000000E+000
//# TD 0.000000000E+000 0.000000000E+000 0.000000000E+000 0.000000000E+000 0.000000000E+000 0.000000000E+000 2.500000000E+001
//# OD -1.00000 -1.00000 -1.00000 -1.00000 -1.00000 -1.00000
//# LD 3.34000000E-001 2.32500000E+000
//# IT 3.34000E-001 1.00000E+000 2.50000E+001

#include "../AcornGlass.hh"

arec::typedef ::acorn::GlassCat {
    char*	name
    char*	catalog

    vector<AcornGlass> glass;

    AcornGlassCat(char *filename) {
	AGF agf();

	return agf::Read(filename);
    }
};


class AGF {
    static std::map<std::string, void (ZMX::*)(std::vector<char*>)> mtable;

  public:
    vector<AcornGlass> glass;

    char *CC

    void invoke(const char *method, std::vector<char*> argv) {
	if ( mtable.count(method) == 1 ) {
	    (this->*mtable[method])(argv);
	} else {
	     fprintf(stderr, "unknown method ZMX::%s\n", method);
	}
    }


    AcornCat *Read(string filename) {
	AcronGlassCat catalog = new AcornGlassCat();

	std::vector<char*> list = split(&cat(filename)[0], "\n");

        for ( auto &i : list ) {
	    std::vector<char*> line = split(i, " \t");

	    invoke(line[0], line);
        }

	return model;
    }

    Keyword CC (std::vector<char*> argv) {
	CC = argv[1];
    }

    Keyword NM (std::vector<char*> argv) {
	glass.emplace_back();

	glass.back.name 	=      argv[1];
	glass.back.formula 	= stoi(argv[2]);
	glass.back.MIL 		= stod(argv[3]);
	glass.back.Nd		= stod(argv[4]);
	glass.back.Vd 		= stod(argv[5]);
	glass.back.exclude 	=      argv[6];
	glass.back.status 	=      argv[7];
    }

    Keyword GC (std::vector<char*> argv) {
	glass.back.comment 	=      argv[1];
    }
    Keyword ED (std::vector<char*> argv) {
	glass.back.TCE 		= stod(argv[1]);
	glass.back.TCE100300 	= stod(argv[2]);
	glass.back.density 	= stod(argv[3]);
	glass.back.dPgf 	= stod(argv[4]);
	glass.back.ignthermal 	= stod(argv[5]);
    }

    Keyword CD (std::vector<char*> argv) {
	for ( i = 0; i < argv.size(); i++ ) {
	    glass.back.c[i]	= stod(argv[1]);
	}
    }
    Keyword TD (std::vector<char*> argv) {
	glass.back.D0 		= stod(argv[1]);
	glass.back.D1 		= stod(argv[2]);
	glass.back.D2 		= stod(argv[3]);
	glass.back.E0 		= stod(argv[4]);
	glass.back.E1 		= stod(argv[5]);
	glass.back.Ltk 		= stod(argv[6]);
	glass.back.temp 	= stod(argv[7]);
    }

    Keyword OD (std::vector<char*> argv) {}

    Keyword LD (std::vector<char*> argv) {
	glass.back.ymin 	= stod(argv[1]);
	glass.back.ymax 	= stod(argv[2]);
    }

    Keyword IT (std::vector<char*> argv) {}
}

std::map<std::string, void (AGF::*)(std::vector<char*>)> AGF::mtable = {
#	include "agf.mtable"
};

