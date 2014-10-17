
#include <stdio.h>


#include <map>
#include <vector>
#include <string>

#include <Eigen/Dense>

using namespace Eigen;

#include "Acorn.hh"

int main(int argc, char **argv) {

    AcornSurface *surf = AcornSurfaceConstructor("simple");

    fprintf(stderr, "Got : %s %ld\n", surf->type.c_str(), surf->vtable->size());

    surf->setparam("aper_min", 4.0);

    fprintf(stderr, "%f\n", surf->aper_min);

    return 0;
}
