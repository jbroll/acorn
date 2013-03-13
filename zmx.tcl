# Zemax ZMX file.
#
# 
source tcloo.tcl
source unix.tcl

lappend auto_path lib

array set ZMXSurfaceMap {
    standard	 simple
    coordbrk	 coordbrk
    us_array.dll lens-array-rect
    szernpha	 zernike
    nsc_ssur	 simple
    nsc_zsur	 zernike
    nsc_annu	 simple
}

array set ZMXNSODMap {
    simple,1	R
    simple,2	K
    simple,3	aper_max

    zernike,1	R
    zernike,2	K
    zernike,3	aper_max

}

array set ZMXUserSurfMap {
    lens-array,1	nx
    lens-array,2	ny
    lens-array,3	width
    lens-array,4	height
}

oo::class create ZMX {
    superclass ::acorn::BaseModel

    variable grouptype current surf surftype surfaces default basedef basemap anonsurf		\
	Id Name Notes Temp Pres									\
	params comment nonseqid nonseq nsoexit

    accessor grouptype current surfaces default

    constructor { type args } {
	procs TRAC BLNK CLAP COAT COFN COMM CONF CONI CURV DIAM DISZ DMFS EFFL ENVD FLAP FLOA FTYP FWGN GCAT GFAC GLAS GLCZ GLRS GSTD HIDE IGNR MAZH MIRR MNUM MODE NAME NOTE	\
	      NSCD NSCS NSOA NSOD NSOH NSOO NSOP NSOQ NSOS NSOU NSOV NSOW PARM PFIL PICB POLS POPS PUSH PWAV PZUP RAIM ROPD SDMA SLAB STOP SURF		\
	      TOL  TOLE TYPE UNIT VANN VCXN VCYN VDSZ VDXN VDYN VERS WAVM XDAT XFLN YFLN RSCE MOFF

	set grouptype sequential
	set basedef { type simple n 1.00 }
	set default {}
	set basemap { 	type type R R K K n n thickness thickness
	    		aperture aperture aper_type aper_type aper_param aper_param aper_min aper_min aper_max aper_max
			x x y y z z rx rx ry ry rz rz }
	set anonsurf 0

	set surftype {}
	set current [::acorn::Surfs create [namespace current]::surfs[incr [namespace current]::SURFS] 0]
	set surf 0

	switch $type {
	    source { eval [string map { $ \\$ ; \\; [ \\[ } [cat [lindex $args 0]]] }
	    string { eval {*}$args }
	}

	if { [$current length] } {
	    lappend surfaces $grouptype $current
	} else {
	    rename $current {}
	}
    }

    method VERS { args } {}
    method UNIT { lens_unit src_prefix src_unit anal_prefix anal_unit args }	{}
    method ENVD { temp pres args }	{ set Temp $temp; set Pres $pres }
    method GCAT { args }	{}

    method NAME { args } { set     Name $args }
    method NOTE { args } { lappend Notes $args }

    method  SURF { id } { set Id $id;  set comment {} }

     # Common surface definition commands
     #
     method TYPE { type args } { 

	# Move on to a new surface group?
	#
	if { [$current length] && ( $grouptype eq "non-sequential" || $type eq "NONSEQCO" ) } {
	    lappend surfaces $grouptype $current 

	    set current [::acorn::Surfs create [namespace current]::surfs[incr [namespace current]::SURFS] 0]
	    set surf   0
	}

	if { $type eq "NONSEQCO" } {
	    set grouptype non-sequential

	    set nonseqid $Id
	    set nonseq   0
	    return
	}

	set grouptype sequential
	my Process-Type $type {*}$args
     }

     method Process-Type { type args } {
	if { $type eq "USERSURF" } {
	    set args [lassign $args type]
	}
	set args {}

	set type $::ZMXSurfaceMap([string tolower $type])
	switch $type {
	    lens-array-rect {
		set type lens-array
		set args { symetry rect }
	    }
	    lens-array-hex {
		set type lens-array
		set args { symetry hex }
	    }
	}

	set surftype $type


	set surf [$current length]

	$current $surf set name $Id

	# Get the surface traverse and infos functions
	#
	$current $surf set traverse $::acorn::SurfaceTypes($type)
	$current $surf set infos    $::acorn::SurfaceInfos($type)

	if { [$current $surf get infos] != -1 } {
	    lassign [::acorn::infos 1 [$current $surf get infos]] dparams dvalues
	    lassign [::acorn::infos 2 [$current $surf get infos]] sparams svalues
	} else {
	    set dparams {}
	    set dvalues {}
	    set sparams {}
	    set svalues {}
	}

	set k 0
	set parmap {}
	foreach param $dparams { lappend parmap $param p$k; incr k }	; # Query for and map parameters


	set k 0
	foreach param $sparams { lappend parmap $param s$k; incr k }	; # Query for and map strings

	set parmap [dict merge $basemap $parmap]

	$current $surf set {*}[dict merge $basedef $default [mappair $parmap [join $args]]]

	::oo::objdefine [self] [list forward $Id [self] surfset1 $current $surf $parmap]
	::oo::objdefine [self] [list export  $Id]


	if { $comment ne {} } {
	    set name [string map { { } {} } [join [map word $comment { string totitle $word }]]]

	    ::oo::objdefine [self] [list forward $name [self] surfset1 $current $surf $parmap]
	    ::oo::objdefine [self] [list export  $name]
	}

     }
     method CURV { curv  args } { if { $curv } { $current $surf set R [expr { 1.0/$curv }] }  	 }
     method CONI { conic args } { $current $surf set K $conic 	 }
     method COMM { args }  { set comment $args }
     method PARM { n value } { $current $surf set p$n $value }
     method DISZ { thick } { 
	 if { $grouptype ne "non-sequential" } { $current $surf set thickness $thick }
     }
     method DIAM { diam args } {	# This is Zemax computed semi-diameter, not the aperture size.  }
     method SQOB { args } { # aperture obscuration is true }
     method OBSC { args } { # aperture obscuration is true }
     method ELOB { args } { # aperture obscuration is true }
     method SQAP { w h  } { 
     	$current $surf set aper_type rectangular 
     	$current $surf set aper_max  [expr $w/2.0] 
     	$current $surf set aper_min  [expr $h/2.0] 
     }
     method ELAP { w h  } {
     	$current $surf set aper_type eliptical 
     	$current $surf set aper_max  [expr $w/2.0] 
     	$current $surf set aper_min  [expr $h/2.0] 
     }
     method CLAP { n rad args  } {
	    $current $surf set aper_type circular 
	    $current $surf set aper_max  $rad 
     }
     method FLAP { n rad args  } {}
     method OBDC { x y  } { # aperture decenter }

     method GLAS { name args } {
	 $current $surf set glass $name
	 $current $surf set glass_ptr [glass-lookup $name]
     }

     method BLNK { args } {}
     method TRAC { args } {}
     method MOFF { args } {}
     method GLCZ { args } {}
     method RSCE { args } {}


     # NonSequential surface commands
     #
     method NSOH { type args } {
	switch $type {
	 default {
	    if { $nonseq == 0 } { set nsoexit [$current 0 get p0 p1 p2 p3 p4 p5 p6] }

	    $current length $nonseq

	    lassign $args a b comment

	    my SURF $nonseqid-[incr nonseq]
	    my COMM $comment
	    my Process-Type $type

	    $current $surf set thickness [lindex $nsoexit 0 3]
	 }
	}
     }
     method NSOA { n aperture } {
	if { $aperture eq {} } { return }
	if { $aperture eq {{}} } { return }

	$current $surf set aper_type  UDA
	$current $surf set aper_param [string map { {"} {} } $aperture]
	$current $surf set aperture [::acorn::Aperture [$current get aper_type] [$current get aper_param]]
     }
     method NSCS { args } {}
     method NSOD { n value a b c d e f } {
	 catch { $current $surf set $::ZMXNSODMap($surftype,$n) $value } reply
     }
     method NSOP { dx dy dz rx ry rz args } {
	 $current $surf set x $dx y $dy z $dz rx $rx ry $ry rz $rz

	 if { [lindex $args 0] eq "MIRROR" } {
	     $current $surf set n -1 
	 }
     }
     method NSCD { args } {}
     method NSOO { args } {}
     method NSOQ { args } {}
     method NSOS { args } {}
     method NSOU { args } {}
     method NSOV { args } {}
     method NSOW { args } {}


    method EFFL { args } {}
    method COAT { args } {}
    method COFN { args } {}
    method CONF { args } {}
    method DMFS { args } {}
    method FLOA { args } {}
    method FTYP { args } {}
    method FWGN { args } {}
    method GFAC { args } {}
    method GLRS { args } {}
    method GSTD { args } {}
    method HIDE { args } {}
    method IGNR { args } {}
    method MAZH { args } {}
    method MIRR { args } {}
    method MNUM { args } {}
    method MODE { args } {}

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
    method TOL  { args } {}
    method TOLE { args } {}
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


