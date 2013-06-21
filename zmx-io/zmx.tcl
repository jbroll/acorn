# Zemax ZMX file.
#
# 

namespace eval acorn {

    array set ZMXSurfaceMap {
	standard	 	simple
	coordbrk	 	coordbrk
	evenasph     		evenasph
	dgrating	 	dgrating
	szernpha	 	zernike
	szernsag	 	zernike

	nsc_ssur	 	simple
	nsc_zsur	 	zernike
	nsc_annu	 	simple

	us_array.dll 	lens-array-rect
	us_hexarray.dll	lens-array-hex
    }

    array set ZMXNSODMap {
	simple,1	R
	simple,2	K
	simple,3	aper_max

	zernike,1	R
	zernike,2	K
	zernike,15	nterms
	zernike,5	xdecenter
	zernike,6	ydecenter

    }

    array set ZMXParmMap {
	lens-array-rect,1	nx
	lens-array-rect,2	ny
	lens-array-rect,3	width
	lens-array-rect,4	height

	coordbrk,1		x
	coordbrk,2		y
	coordbrk,3		rx
	coordbrk,4		ry
	coordbrk,5		rz

	evenasph,1		a2
	evenasph,2		a4
	evenasph,3		a6
	evenasph,4		a8
	evenasph,5		a10
	evenasph,6		a12
	evenasph,7		a14
	evenasph,8		a16

	dgrating,1		l/mm
	dgrating,2		order
    }

    proc zmx-simple   { model surf } {}
    proc zmx-coordbrk { model surf } {}
    proc zmx-zernike  { model surf } {}
    proc zmx-dgrating { model surf } {}
    proc zmx-lens-array-rect { model surf } {}
    proc zmx-evenasph { model surf } {
	$model $surf set nterms 8
    }

}

oo::class create ::acorn::ZMX {
    superclass ::acorn::BaseModel

    variable grouptype current surf surftype surfaces default basedef basemap basepar anonsurf surfdefs 	\
	pup mce mce_current												\
	Id Name Notes Temp Pres											\
	params comment nonseqid nonseq nsoexit									\
	debug

    accessor grouptype current surfaces default debug

    constructor { type args } {
	next

	set pup    {}
	set mce(1) {}

	set debug 0

	procs TRAC BLNK CLAP COAT COFN COMM CONF CONI CURV DIAM DISZ DMFS EFFL ENVD ENPD FLAP FLOA FTYP FWGN FWGT GCAT GFAC GLAS GLCZ GLRS GSTD HIDE IGNR MAZH MIRR MNUM MODE NAME NOTE	\
	      NSCD NSCS NSOA NSOD NSOH NSOO NSOP NSOQ NSOS NSOU NSOV NSOW PARM PPAR PRAM PFIL PICB POLS POPS PUSH PUPD PWAV PZUP RAIM ROPD SCOL SDMA SLAB STOP SURF		\
	      TOL  TOLE TYPE UNIT VANN VCXN VCYN VDSZ VDXN VDYN VERS WAVL WAVN WAVM WWGT WWGN XDAT XFLD XFLN YFLD YFLN XFIE RSCE RWRE MOFF OBSC SQAP ELOB WAVE THIC ZVDX ZVDY ZVCX ZVCY ZVAN

	switch $type {
	    source { eval [string map { $ \\$ ; \\; [ \\[ } [cat [lindex $args 0]]] }
	    string { eval {*}$args }
	}

	if { [$current length] } {
	    lappend surfaces $grouptype $current
	} else {
	    rename $current {}
	}


	eval $pup
	eval $mce($mce_current)
    }

    method simple { args } {}
    method zernike { n value a b c d e f } {
	if { $n == 3 } {
	    my [$current $surf get name] set aper_max $value
	    my [$current $surf get name] set nradius  $value
	}
	if { $n > 15 } {
	    my [$current $surf get name] set z[expr $n-15] $value
	}
    }

    method pickup { } { eval $pup }

    method config { { config {} } } {
	if { $config eq {} } { 
	    return $mce_current
	} else {
	    set mce_current $config
	    eval $mce($config)
	    eval $pup
	}
    }

    method ZVAN { args } {}
    method ZVCX { args } {}
    method ZVCY { args } {}
    method ZVDX { args } {}
    method ZVDY { args } {}
    method OBSC { args } {}
    method VERS { args } {}
    method UNIT { lens_unit src_prefix src_unit anal_prefix anal_unit args }	{}
    method ENVD { temp pres args }	{ set Temp $temp; set Pres $pres }
    method ENPD { args }	{ }
    method GCAT { args }	{}

    method NAME { args } { set     Name $args }
    method NOTE { args } { lappend Notes $args }

    method  SURF { id } { set Id $id;  set comment {} }

    method xNOP { args } {
	#puts "NOP $args"
    } 

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

	    ::oo::objdefine [self] [list forward $Id [self] xNOP $Id]
	    ::oo::objdefine [self] [list export  $Id]

	    set nonseqid $Id
	    set nonseq   0
	    return
	}

	set grouptype sequential
	my Process-Type $type $comment $args
     }

     method Process-Type { type comm args } {
	if { $type eq "USERSURF" } {
	    set args [lassign $args type]
	}
	set args {}

	set type $::acorn::ZMXSurfaceMap([string tolower $type])		; # Map Zemax surface type in to acorn.


	set surf [$current length]

	set surftype $type

	# Get the surface traverse and infos functions
	#
	$current $surf set type     $type
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

	set surfdefs($surftype,pdef) [dict merge $basedef [zip $dparams $dvalues]]
	set surfdefs($surftype,sdef) [dict merge [zip $sparams $svalues]]

	set k [llength $basepar]
	set parmap {}
	foreach param $dparams { lappend parmap $param p$k; incr k }	; # Query for and map parameters


	set k 0
	foreach param $sparams { lappend parmap $param s$k; incr k }	; # Query for and map strings

	set parmap [dict merge $basemap $parmap]

	set surfdefs($surftype,pmap) $parmap
	set surfdefs($surftype,smap) {}

	$current $surf set {*}[mappair $parmap [dict merge $basedef $default [join $args] [list name $Id type $surftype comment $comm]]]

	::oo::objdefine [self] [list forward $Id [self] surfset1 $current $surf $parmap]
	::oo::objdefine [self] [list export  $Id]


	if { $comment ne {} } {
	    set name [string map { { } {} } [join [map word $comment { string totitle $word }]]]

	    ::oo::objdefine [self] [list forward $name [self] surfset1 $current $surf $parmap]
	    ::oo::objdefine [self] [list export  $name]

	}

	::acorn::zmx-$surftype [self] $Id
     }
     method CURV { curv  args } {
	 if { $grouptype ne "non-sequential" } {
	     if { $curv } {
		 my [$current $surf get name] set R [expr { 1.0/$curv }]
	     } 
	 }
     }
     method CONI { conic args } {                my [$current $surf get name] set K $conic 	 }
     method COMM { args }  { set comment $args }

     method PARM { n value } {
	 if { $grouptype ne "non-sequential" } {
	     try { my [$current $surf get name] set $::acorn::ZMXParmMap($surftype,$n) $value
	     } on error message {
		 if { $debug } { puts stderr "PARM $surftype $n $value : $message" }
	     }
	 } else {
	     $current $surf set p$n $value
	 }
     }
     method DISZ { thick } { 
	 if { $grouptype ne "non-sequential" } { $current $surf set {*}[mappair $basemap [list thickness $thick]] }
     }
     method DIAM { diam args } {	# This is Zemax computed semi-diameter, not the aperture size.  }
     method SQOB { args } { # aperture obscuration is true }
     method OBSC { args } { # aperture obscuration is true }
     method ELOB { args } { # aperture obscuration is true }
     method SQAP { w h args  } { 
     	$current $surf set aper_type rectangular 
     	my [$current $surf get name] set aper_max  [expr $w/2.0] 
     	my [$current $surf get name] set aper_min  [expr $h/2.0] 
     }
     method ELAP { w h  } {
     	$current $surf set aper_type eliptical 
     	$current $surf set aper_max  [expr $w/2.0] 
     	$current $surf set aper_min  [expr $h/2.0] 
     }
     method CLAP { n rad args  } {
	    $current $surf set aper_type circular 
	    my [$current $surf get name] set aper_max  $rad 
     }
     method FLAP { n rad args  } {}
     method OBDC { x y  } { # aperture decenter }

     method GLAS { name args } {
	 $current $surf set glass $name
	 try {
	     $current $surf set glass_ptr [glass-lookup $name]
	 } on error message {
	     puts "missing glass : $name"
	 }
     }

     method BLNK { args } {}
     method TRAC { args } {}
     method MOFF { args } {}
     method GLCZ { args } {}
     method RSCE { args } {}
     method RWRE { args } {}


     # NonSequential surface commands
     #
     method NSOH { type args } {
	switch $type {
	 default {
	    if { $nonseq == 0 } {
		set nsoexit [$current 0 get p0 p1 p2 p3 p4 p5 p6]
		$current 0 set p0 0 p1 0 p2 0 p3 0 p4 0 p5 0 p6 0
	    }

	    $current length $nonseq

	    my SURF $nonseqid-[incr nonseq]

	    lassign $args a b comment

	    my Process-Type $type $comment

	    my [$current $surf get name] set thickness [lindex $nsoexit 3]
	 }
	}
     }
     method NSOA { n aperture } {

	if { $aperture eq  {}  } {
	    $current $surf set aper_type  circular
	    return
	}

	$current $surf set aper_type  UDA
	$current $surf set aper_param [string map { {"} {} } $aperture]
	$current $surf set aperture [::acorn::Aperture [$current $surf get aper_type] [$current $surf get aper_param]]
     }
     method NSCS { args } {}
     method NSOD { n value a b c d e f } {
	 try { my [$current $surf get name] set $::acorn::ZMXNSODMap($surftype,$n) $value 
	 } on error message {
	     my [$current $surf get type] $n $value $a $b $c $d $e $f
	 }
     }
     method NSOP { dx dy dz rx ry rz args } {
	 my [$current $surf get name] set x $dx y $dy z $dz rx $rx ry $ry rz $rz

	 if { [lindex $args 0] eq "MIRROR" } {
	     my [$current $surf get name] set n -1 
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
    method FWGT { args } {}
    method FWGN { args } {}
    method GFAC { args } {}
    method GLRS { args } {}
    method GSTD { args } {}
    method HIDE { args } {}
    method MAZH { args } {}
    method MIRR { args } {}
    method MODE { args } {}

    method PFIL { args } {}
    method PICB { args } {}
    method POLS { args } {}
    method POPS { args } {}
    method PUSH { args } {}
    method PUPD { args } {}
    method PWAV { args } {}
    method RAIM { args } {}
    method ROPD { args } {}
    method SCOL { args } {}
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
    method WAVL { args } {}
    method WAVM { args } {}
    method WAVN { args } {}
    method WWGT { args } {}
    method WWGN { args } {}
    method XDAT { args } {}
    method XFLD { args } {}
    method XFLN { args } {}
    method YFLN { args } {}
    method YFLD { args } {}

    # Pickups
    #
    method PZUP {       from scale offset column } { append pup "my xPZUP $Id [expr int($from)] $scale $offset $column\n" }
    method PPAR { param from scale offset column } { append pup "my xPPAR $Id [expr int($from)] $scale $offset $column $param\n" }

    method xPZUP { surf from scale offset column } { my $surf thickness set [expr [my $from get thickness]*$scale] }
    method xPPAR { surf from scale offset column param } {
	set value [my $from get $acorn::ZMXParmMap([my $surf get type],$param)]*$scale+$offset]

	my     $surf set $acorn::ZMXParmMap([my $surf get type],$param) 	\
	  [expr [my $from get $acorn::ZMXParmMap([my $surf get type],$param)]*$scale+$offset]
    }


    # Multi Configuration Editor
    #
    method MNUM { n { curr 1 } } { set mce_current $curr }					; # Multi Configure Number of configs

    method WAVE { surf config args } { append mce($config) "my xWAVE $surf $args\n" }	; # Set Wavelength
    method IGNR { surf config args } { append mce($config) "my xIGNR $surf $args\n" }	; # Ignore surface
    method PRAM { surf config args } { append mce($config) "my xPRAM $surf $args\n" }	; # Set Parameter
    method XFIE { surf config args } { append mce($config) "my xXFIE $surf $args\n" }
    method THIC { surf config args } { append mce($config) "my xTHIC $surf $args\n" }

    method xIGNR { surf value args }         { my $surf set enable [expr !int($value)] }
    method xPRAM { surf value x param args } { my $surf set $acorn::ZMXParmMap([my $surf get type],$param) $value }
    method xTHIC { surf value args }         { my $surf set thickness                                      $value }
    method xXFIE { surf value args } { }
    method xWAVE { surf value args } { }
}

