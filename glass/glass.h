
typedef struct _GlassEntry {
    char*	comment;
    char*	name;
    int		formula;
    int		MIL;
    double	Nd;
    double	Vd;
    double	TCE;
    double	TCE100300;
    double	density;
    double	dPgF;
    double	c[10]
    double	D0, D1, D2, E0, E1, Ltk, temp;
    double	ymin, ymax;
} GlassEntry;

typedef struct _GlassCatalogs {
    char*	catalog;
    GlassEntry	*glass;
} GlassCatalogs;
