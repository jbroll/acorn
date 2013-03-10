
#include "glass.h"

int GlasSize() { return sizeof(struct _GlassEntry); }

double glass_indx(GlassEntry *glass, double wave) {
    return glass_index(glass->formula, wave, 25.0, 1.0, glass->c);
}

