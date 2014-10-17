
static int Traverse(AcornRay *rays) { return 0; }

AcornSurfGrp *AcornSurfGrpConstructor(void) {
    AcornSurfGrp *surf = new AcornSurfGrp();

    surf->traverse = Traverse;
    surf->vtable   = &vtable;

    return surf;
}

static CStrVarMapMap VTable = {
#	include "AcornSurfGrp.vtable"
};
