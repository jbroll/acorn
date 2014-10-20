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



arec::typedef ::acorn::GlassCat {
    char*	name
    char*	catalog
    char*	glass
};


class AGF {
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
	$glass set $current name $name formula $formula MIL $MIL Nd $Nd Vd $Vd exclude $exclude status $status
    }
    method GC { args } { $glass set $current comment [join $args] }
    method ED { TCE TCE100300 density dPgF ignthermal args } {
	$glass set $current TCE $TCE TCE100300 $TCE100300 density $density dPgF $dPgF ignthermal $ignthermal
    }
    method CD { args } {
	set i 0
	foreach c $args { $glass set $current c$i $c; incr i }
    }
    method TD { D0 D1 D2 E0 E1 Ltk temp  } {
	$glass set $current D0 $D0 D1 $D1 D2 $D2 E0 $E0 E1 $E1 Ltk $Ltk temp $temp
    }
    method OD { args } {}
    method LD { ymin ymax  } {
	$glass set $current ymin $ymin ymax $ymax
    }
    method IT { args } {}
}


proc glass-loader { pathlist } {
    ::acorn::GlassCat create GlassCats 0

    foreach path $pathlist {
	foreach catalog [glob $pathlist/*.agf] {
	    set agf [AGF create agf[incr ::AGF] source $catalog]

	    GlassCats set end+1 name [file rootname $catalog] catalog $catalog glass $agf
	}
     }

     set ::Glass() 0
     set ::Glass({}) 0
     set ::Glass({{}}) 0
     set ::Glass({{{}}}) 0
     set ::Glass((null)) 0

     set ::Glass(MIRROR) -1

     foreach cat [GlassCats get glass] {

         set i -1
         foreach name [[$cat glass] get name] {
	     set ::Glass([lindex $name 0]) [[$cat glass] getptr [incr i]]
	 }
     }
}

proc glass-lookup { glass } {	return $::Glass($glass) }

