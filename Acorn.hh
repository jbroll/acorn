#ifndef Acorn_HH
#define Acorn_HH

#define X__PI	3.14159265358979323846
#define X_2PI	( 2 * X__PI )
#define X_R2D	(X_2PI / 360.0)
#define X_R2H	(X_2PI /  24.0)
#define X_H2D	(360.0 /  24.0)

#define r2h(r)	( (r) / X_R2H )
#define h2r(d)	( (d) * X_R2H )
#define r2d(r)	( (r) / X_R2D )
#define d2r(d)	( (d) * X_R2D )
#define h2d(r)	( (r) * X_H2D )
#define d2h(d)	( (d) / X_H2D )

#define ACORN_ITER	15
#define Keyword void
#define Param

#define Type_int       1
#define Type_string    2
#define Type_double    3

struct VarMap {
    int type;
    int offset;

    VarMap() {
	type   = 0;
	offset = 0;
    }
    VarMap(int Type, int Offset) {
	type   = Type;
	offset = Offset;
    };
};



#include <string.h>

#include <string>
#include <vector>
#include <map>

struct cstrcmp {
    bool operator()(const char *a, const char *b) const {
	return strcmp(a, b) < 0;
    }
};



typedef std::string string;

typedef void (*SagittaFunc)(struct AcornSurface *s, double x, double y, double *dz, double *dx, double *dy);

#include <Eigen/Dense>

using namespace Eigen;

#include "AcornRay.hh"
#include "AcornUtil.hh"
#include "AcornSurface.hh"
#include "AcornGlass.hh"
#include "AcornSurfGrp.hh"
#include "AcornModel.hh"

#endif
