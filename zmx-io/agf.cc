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

#include "../Acorn.hh"
#include "invoker.hh"


class AGF {
    INVOKER(AGF)

    std::vector<AcornGlass> *glass;
    char *cc;


    Keyword CC (std::vector<char*> argv) {
	cc = argv[1];
    }

    Keyword NM (std::vector<char*> argv) {
	glass->emplace_back();

	glass->back().name 	=      argv[1];
	glass->back().formula 	= std::stoi(argv[2]);
	glass->back().MIL 	= std::stod(argv[3]);
	glass->back().Nd		= std::stod(argv[4]);
	glass->back().Vd 	= std::stod(argv[5]);
	glass->back().exclude 	= std::stod(argv[6]);
	glass->back().status 	= std::stod(argv[7]);
    }

    Keyword GC (std::vector<char*> argv) {
	glass->back().comment 	=      argv[1];
    }
    Keyword ED (std::vector<char*> argv) {
	glass->back().TCE 		= std::stod(argv[1]);
	glass->back().TCE100300 	= std::stod(argv[2]);
	glass->back().density 	= std::stod(argv[3]);
	glass->back().dPgF 	= std::stod(argv[4]);
	glass->back().ignthermal 	= std::stod(argv[5]);
    }

    Keyword CD (std::vector<char*> argv) {
	for ( int i = 0; i < argv.size(); i++ ) {
	    glass->back().c[i]	= std::stod(argv[1]);
	}
    }
    Keyword TD (std::vector<char*> argv) {
	glass->back().D0 		= std::stod(argv[1]);
	glass->back().D1 		= std::stod(argv[2]);
	glass->back().D2 		= std::stod(argv[3]);
	glass->back().E0 		= std::stod(argv[4]);
	glass->back().E1 		= std::stod(argv[5]);
	glass->back().Ltk 		= std::stod(argv[6]);
	glass->back().temp 	= std::stod(argv[7]);
    }

    Keyword OD (std::vector<char*> argv) {}

    Keyword LD (std::vector<char*> argv) {
	glass->back().ymin 	= std::stod(argv[1]);
	glass->back().ymax 	= std::stod(argv[2]);
    }

    Keyword IT (std::vector<char*> argv) {}


    std::vector<AcornGlass> *Read(string filename) {
	glass = new std::vector<AcornGlass>;

	std::vector<char*> list = split(&cat(filename)[0], "\n");

        for ( auto &i : list ) {
	    if ( i[0] == '!' || i[0] == '#' ) { continue; }		// Allow comments

	    std::vector<char*> line = split(i, " \t");

	    invoke(line[0], line);
        }

	return glass;
    }
};

std::map<std::string, void (AGF::*)(std::vector<char*>)> AGF::mtable = {
#	include "agf.mtable"
};

struct AcornGlassCat {
    std::string name;

    std::vector<AcornGlass> *glass;

    AcornGlassCat(const char *filename) {
	AGF agf;

	name  = filename;

	glass = agf.Read(filename);
    }
};

