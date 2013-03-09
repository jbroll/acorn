# Zemax ZMX file.
#
# 
lappend auto_path lib

source tcloo.tcl
source unix.tcl

package require arec

oo::class create ZMX {
    variable model
    accessor glass

    constructor { type args } {
	procs CLAP COAT COFN COMM CONF CURV DIAM DISZ DMFS ENVD FLAP FLOA FTYP FWGN GCAT GFAC GLAS GLRS GSTD HIDE IGNR MAZH MIRR MNUM MODE NAME NOTE	\
	      NSCD NSCS NSOA NSOD NSOH NSOO NSOP NSOQ NSOS NSOU NSOV NSOW PARM PFIL PICB POLS POPS PUSH PWAV PZUP RAIM ROPD SDMA SLAB STOP SURF		\
	      TOL  TYPE UNIT VANN VCXN VCYN VDSZ VDXN VDYN VERS WAVM XDAT XFLN YFLN

	switch $type {
	    source { eval [string map { $ \\$ ; \\; [ \\[ } [cat [lindex $args 0]]] }
	    string { eval {*}$args }
	}

    }
    method UNIT { args } {}
    method VERS { args } {}

    method CLAP { args } {}
    method COAT { args } {}
    method COFN { args } {}
    method COMM { args } {}
    method CONF { args } {}
    method CURV { args } {}
    method DIAM { args } {}
    method DISZ { args } {}
    method DMFS { args } {}
    method ENVD { args } {}
    method FLAP { args } {}
    method FLOA { args } {}
    method FTYP { args } {}
    method FWGN { args } {}
    method GCAT { args } {}
    method GFAC { args } {}
    method GLAS { args } {}
    method GLRS { args } {}
    method GSTD { args } {}
    method HIDE { args } {}
    method IGNR { args } {}
    method MAZH { args } {}
    method MIRR { args } {}
    method MNUM { args } {}
    method MODE { args } {}
    method NAME { args } {}
    method NOTE { args } {}

    method NSCD { args } {}
    method NSCS { args } {}
    method NSOA { args } {}
    method NSOD { args } {}
    method NSOH { args } {}
    method NSOO { args } {}
    method NSOP { args } {}
    method NSOQ { args } {}
    method NSOS { args } {}
    method NSOU { args } {}
    method NSOV { args } {}
    method NSOW { args } {}
    method PARM { args } {}
    method PFIL { args } {}
    method PICB { args } {}
    method POLS { args } {}
    method POPS { args } {}
    method PUSH { args } {}
    method PWAV { args } {}
    method PZUP { args } {}
    method RAIM { args } {}
    method ROPD { args } {}
    method SDMA { args } {}
    method SLAB { args } {}
    method STOP { args } {}
    method SURF { args } {}
    method TOL { args } {}
    method TYPE { args } {}
    method VANN { args } {}
    method VCXN { args } {}
    method VCYN { args } {}
    method VDSZ { args } {}
    method VDXN { args } {}
    method VDYN { args } {}
    method WAVM { args } {}
    method XDAT { args } {}
    method XFLN { args } {}
    method YFLN { args } {}
}


puts [ZMX create GMT source models/GMT-1-AcOWFS-2.ZMX]

