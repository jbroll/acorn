#ifndef AcornSurfGrp_HH
#define AcornSurfGrp_HH

#include <vector>

enum AcornSurfGrpType { AcornSequential, AcornNonSequential };

struct AcornSurfGrp {
   ACORN_SURFACE

    AcornSurfGrpType seqtype;
    std::vector<AcornSurface *> surfaces;

    AcornSurfGrp () { 
	type    = "non-sequential";
	seqtype = AcornSequential;
    }
    AcornSurfGrp (AcornSurfGrpType Type) {
	seqtype = Type;
    }

    AcornSurface *append(AcornSurface *surf) {
	surfaces.push_back(surf);

	return surf;
    }

};
#endif
