#ifndef AcornGlass_HH
#define AcornGlass_HH

#include "Acorn.hh"

class AcornGlass {
    Param string	comment;
    Param string	name;
    Param int		formula;
    Param double	MIL;
    Param double	Nd;
    Param double	Vd;
    Param int		exclude;
    Param int		status;
    Param double	TCE;
    Param double	TCE100300;
    Param double	density;
    Param double	dPgF;
    Param int		ignthermal;
    Param double	D0;
    Param double	D1;
    Param double	D2;
    Param double	E0;
    Param double	E1;
    Param double	Ltk;
    Param double	temp;
    Param double	ymin;
    Param double	ymax;

    double	c[10];
};

#endif
