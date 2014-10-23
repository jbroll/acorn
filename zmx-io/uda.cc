// Zemax UDA file.
//
// 
// ARC cx cy angle n
// BRK
// CIR cx cy radius n
// ELI cx cy rx ry angle n
// LIN x y n
// x y
// POL cx cy radius n angle
// REC cx cy w h angle
// ! Comment

#include "../Acorn.hh"
#include "invoker.hh"


struct Polygon {
    double	x;
    double	y;
    double	z;

    Polygon() {
	x = 0.0;
	y = 0.0;
	z = 0.0;
    }
    Polygon(const Polygon &p) {
	x = p.x;
	y = p.y;
	z = p.z;
    }
    Polygon(double dx, double dy, double dz=0.0) {
	x = dx;
	y = dy;
	z = dz;
    }
    Polygon(const char *sx, const char *sy, const char *sz="0") {
	x = std::stod(sx);
	y = std::stod(sy);
	z = std::stod(sz);
    }
};

#include "../Acorn.hh"
#include "invoker.hh"

// The UDA file is read in and reduced to a polygon definition
//
class UDA {
    INVOKER(UDA)

    std::vector<Polygon> *polygon;

    int		inpath;
    bool	brk;

    void rpoly (double cx, double cy, double radius, int n) {
	_BRK();

	//foreach { x y } [rp cx cy radius n] {
	//    $polygon set end+1 x $x y $y
	//}
    }

    Keyword CIR (std::vector<char*> argv) {
	//double cx, double cy, double r, int n=64 ) 	{ rpoly $cx $cy $r $n }
    }
    Keyword ELI (std::vector<char*> argv) {
	//double cx, double cy, double rx, double ry, double rot, int n=0)  
    }
    Keyword REC (std::vector<char*> argv) {
	//double cx, double cy, double w, double h, double rot } 
    }
    Keyword POL (std::vector<char*> argv) {
	//double cx, double cy, double radius, double n, double rot)
    }

    Keyword ARC (std::vector<char*> argv) {
	//if { inpath == -1 } { inpath = polygon->size(); }
    }
    Keyword LIN (std::vector<char*> argv) {
	if ( inpath == -1 ) { inpath = polygon->size(); }

	polygon->emplace_back(argv[1], argv[2]);
    }
    Keyword BRK (std::vector<char*> argv) { _BRK(); }
    void    _BRK (void) {
	if ( inpath != -1 ) { polygon->emplace_back((*polygon)[inpath]); }
	if ( brk          ) { polygon->emplace_back(); }

	inpath = -1;
	brk    =  0;
    }

    Keyword unknown (std::vector<char*> argv) {

	switch ( argv[0][0] ) {
	    case '-' : case '+' : case '.' :
	    case '0' : case '1' : case '2' : case '3' : case '4' : case '5' : case '6' : case '7' : case '8' : case '9' : {
		double x = std::stod(argv[1]);
		double y = std::stod(argv[2]);

		if ( ( x == 0 && y == 0 ) || ( inpath >= 0 && x == (*polygon)[inpath].x && y == (*polygon)[inpath].y ) ) {
		    _BRK();
		} else {
		    LIN(argv);
		}
		break;
	    }
	}
    }

    std::vector<Polygon> *Read(string filename) {
	polygon = new std::vector<Polygon>;
	inpath  = -1;
	brk     =  true;

	std::vector<char*> list = split(&cat(filename)[0], "\n");

        for ( auto &i : list ) {
	    if ( i[0] == '!' || i[0] == '#' ) { continue; }		// Allow comments

	    for ( int j = 0; i[j] != '\0'; j++ ) {
		if ( i[j] == ',' ) { i[j] = ' '; }			// delete comma.
	    }

	    std::vector<char*> line = split(i, " \t");

	    invoke(line[0], line);
        }
	_BRK();

	return polygon;
    }
};

std::map<std::string, void (UDA::*)(std::vector<char*>)> UDA::mtable = {
#	include "uda.mtable"
};
