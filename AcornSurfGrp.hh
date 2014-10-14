
enum AcornSurfGrpType { AcornSequential, AcornNonSequential };

struct AcornSurfGrp {
   ACORN_SURFACE

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
