# Zemax UDA file.
#
# 
# CC Updated '12.04.24
# NM APL1 1 517696 1.517277 69.563209 0 2 0
# GC 
# ED 0.000000000E+000 0.000000000E+000 0.000000000E+000 0.000000000E+000 0 0
# CD 2.276240300E+000 -9.687464700E-003 9.233037800E-003 3.894085300E-004 -3.801873300E-005 2.069322500E-006 0.000000000E+000 0.000000000E+000 0.000000000E+000 0.000000000E+000
# TD 0.000000000E+000 0.000000000E+000 0.000000000E+000 0.000000000E+000 0.000000000E+000 0.000000000E+000 2.500000000E+001
# OD -1.00000 -1.00000 -1.00000 -1.00000 -1.00000 -1.00000
# LD 3.34000000E-001 2.32500000E+000
# IT 3.34000E-001 1.00000E+000 2.50000E+001

lappend auto_path lib

source tcloo.tcl
source unix.tcl

package require arec

arec::typedef ::acorn::Glass {
    char*	comment;
    char*	name;
    int		formula;
    double	MIL;
    double	Nd;
    double	Vd;
    int		exclude
    int		status
    double	TCE;
    double	TCE100300;
    double	density;
    double	dPgF;
    int		ignthermal
    double	c0 c1 c2 c3 c4 c5 c6 c7 c8 c9 
    double	D0 D1 D2 E0 E1 Ltk temp;
    double	ymin ymax;
}

arec::typedef ::acorn::GlassCat {
    char*	catalog
    long	glass
}


# The UDA file is read in and reduced to a polygon definition
#

oo::class create AGF {
    variable glass current CC
    accessor glass

    constructor { type args } {
	procs CC NM GC ED CD TD OD LD IT

	set glass [acorn::Glass create [namespace current]::glass 0]

	switch $type {
	    source { eval [string map { $ \\$ ; \\; [ \\[ } [cat [lindex $args 0]]] } 
	    string { eval {*}$args }
	}
    }
    method CC { args } { set CC $args }

    method NM { name formula MIL Nd Vd exclude status args } {
	set current [$glass length]
	$glass $current set name $name formula $formula MIL $MIL Nd $Nd Vd $Vd exclude $exclude status $status
    }
    method GC { args } { $glass $current set comment [join $args] }
    method ED { TCE TCE100300 density dPgF ignthermal } {
	$glass $current set TCE $TCE TCE100300 $TCE100300 density $density dPgF $dPgF ignthermal $ignthermal
    }
    method CD { args } {
	set i 0
	foreach c $args { $glass $current set c$i $c; incr i }
    }
    method TD { D0 D1 D2 E0 E1 Ltk temp  } {
	$glass $current set D0 $D0 D1 $D1 D2 $D2 E0 $E0 E1 $E1 Ltk $Ltk temp $temp
    }
    method OD { args } {}
    method LD { ymin ymax  } {
	$glass $current set ymin $ymin ymax $ymax
    }
    method IT { args } {}
}



proc load-glass { glasscats pathlist } {
    foreach path $pathlist {
	foreach catalog [glob $pathlist/*.agf] {
	    AGF create $catalog source glass/schott.agf

	    $galsscats [$galsscats length] set catalog $catalog glass [[$catalog glass] getptr]
	}
    }
}

# ::acorn::GlassCat create GlassCats 0
#
# load-glass GlassCats glass
#
