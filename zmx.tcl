# Zemax ZMX file.
#
# 
source tcloo.tcl
source unix.tcl

lappend auto_path lib

array set ZMXSurfaceMap {
    standard	 simple
    coordbrk	 coordbrk
    us_array.dll lens-array
    nsc_zsur	 zernike
    szernpha	 zernike
}

oo::class create ZMX {
    superclass ::acorn::BaseModel

    variable grouptype current surf surfaces default basedef basemap anonsurf		\
	Id Name Notes Temp Pres								\
	params comment nonseqid nonseq

    accessor grouptype current surfaces default

    constructor { type args } {
	procs CLAP COAT COFN COMM CONF CONI CURV DIAM DISZ DMFS EFFL ENVD FLAP FLOA FTYP FWGN GCAT GFAC GLAS GLCZ GLRS GSTD HIDE IGNR MAZH MIRR MNUM MODE NAME NOTE	\
	      NSCD NSCS NSOA NSOD NSOH NSOO NSOP NSOQ NSOS NSOU NSOV NSOW PARM PFIL PICB POLS POPS PUSH PWAV PZUP RAIM ROPD SDMA SLAB STOP SURF		\
	      TOL  TOLE TYPE UNIT VANN VCXN VCYN VDSZ VDXN VDYN VERS WAVM XDAT XFLN YFLN RSCE MOFF

	set grouptype sequential
	set basedef { type simple n 1.00 }
	set default {}
	set basemap { type type R R K K n n thickness thickness aperture aperture aper_type aper_type aper_param aper_param aper_min aper_min aper_max aper_max x x y y z z rx rx ry ry rz rz }
	set anonsurf 0

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
	if { $type eq "USERSURF" } {
	    set args [lassign $args type]
	}

	# Move on to a new surface group?
	#
	if { [$current length] && ( $grouptype eq "NONSEQCO" || $type eq "NONSEQCO" ) } {
	    lappend surfaces $grouptype $current 

	    set current [::acorn::Surfs create [namespace current]::surfs[incr [namespace current]::SURFS] 0]
	    set surf 0
	}

	if { $type eq "NONSEQCO" } {
	    set grouptype non-sequential

	    set nonseqid $Id
	    set nonseq   0
	    return
	}

	set grouptype sequential

	set type $::ZMXSurfaceMap([string tolower $type])

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

	set name [string map { { } {} } [join [map word $comment { string totitle $word }]]]

	if { $comment ne {} } {
	    ::oo::objdefine [self] [list forward $name [self] surfset1 $current $surf $parmap]
	    ::oo::objdefine [self] [list export  $name]
	}

     }
     method CURV { curv  args } { if { $curv } { $current $surf set R [expr { 1.0/$curv }] }  	 }
     method CONI { conic args } { $current $surf set K $conic 	 }
     method COMM { args }  {
	 set comment $args
	 #$current $surf set comment $args
     }
     method PARM { n value } { $current $surf set p$n $value }
     method DISZ { thick } { $current $surf set thickness $thick }
     method DIAM { diam args } {
	 if { $diam } {
	    $current $surf set aper_type circular 
	    $current $surf set aper_max  [expr $diam/2.0] 
	 }
     }
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

     method MOFF { args } {}
     method GLCZ { args } {}
     method RSCE { args } {}


     # NonSequential surface commands
     #
     method NSOH { type args } {
	switch $type {
	 NSC_ZSUR {
	    lassign $args a b comment
	    my SURF $nonseqid-[incr nonseq]
	    my COMM $comment
	    my TYPE $type
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
     method NSOD { args } {}
     method NSCD { args } {}
     method NSOO { args } {}
     method NSOP { args } {}
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


