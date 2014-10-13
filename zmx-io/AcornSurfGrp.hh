
#include "AcornSurface.hh"

enum AcornSurfGrpType { AcornSequential, AcornNonSequential };

struct AcornSurfGrp {
   ACORNSURFACE

    AcornSurfGrpType seqtype;
    std::vector<AcornSurface *> surf;

    AcornSurfGrp () { 
	type    = "non-sequential";
	seqtype = AcornSequential;
    }
    AcornSurfGrp (AcornSurfGrpType Type) {
	seqtype = Type;
    }

};
