# Zemax ZMX file.
#
# 

class AttrDict(dict):
    def __init__(self, *args, **kwargs):
	super(AttrDict, self).__init__(*args, **kwargs)
	self.__dict__ = self

ZMXSurfaceMap = {
	  "standard":	 	"simple"
	, "coordbrk":	 	"coordbrk"
	, "evenasph ":    	"evenasph"
	, "dgrating":	 	"dgrating"
	, "szernpha":	 	"zernike"
	, "szernsag":	 	"zernike"

	, "nsc_ssur":	 	"simple"
	, "nsc_zsur":	 	"zernike"
	, "nsc_annu":	 	"simple"

	, "us_array.dll": 	"lens-array-rect"
	, "us_hexarray.dll":	"lens-array-hex"
    }

ZMXNSODMap {
      "simple" : ["1": "R", "2": "K", "3": "aper_max"]
    , "zernike": ["", "R", "K", "", "", "xdecenter", "ydecenter", ""
      	        , "", "", "",  "",  "", "",          ""           "nterms"]
}

ZMXParmMap {
	  "lens-array-rect": 	["", "nx", "ny", "width", "height"]
	, "coordbrk":	   	["", "x", "y",  "rx",  "ry",  "rz" ]
	, "evenasph":	   	["", "a2", "a4", "a6", "a8", "a10", "a12", "a14", a16" ]
	, "dgrating"		["", "lmm", "order" ]
}

    proc zmx-evenasph { model surf } {
	$model $surf set nterms 8
    }


def ZMXFile(file):
    return ZMX(unix.cat(file)) 						# Slurp up the file - Fixing up UNICODE

class ZMX(AcornModel):
    def __init__(self, zmx):
	self.state = AttrDict()
	self.state.grouptype = "sequential"

	self.surface   = []

	self.mce_curr  = []
	self.mce_ops   = []

	super(ZMX, self).__init__()

	self.aray = Acorn.Ray(1)

	for line in zmx.split('\n'):
	    line = line.split()

	    if len(line) : getattr(self, line[0])(*line)		# Call internal methods to parse lines

	if float:
	    self.param.pupilDiameter = self.surface[self.stop].semi*2

	self.config(self.mce_current)
    }

    def zernike { n value a b c d e f }
	if n == 3:
	    self.surf.aper_max $value
	    self.surf.nradius  $value

	if n > 15:
	    self.surf["z" + str(n-15)] = value

    def update {} {
	my config $mce_current
	next
    }

    def pickup {} { eval $pup }

    def config(self, config):
	self.mce_current = config

	for line in self.mce[config] : getattr(self, line[0])(*line)
	for line in self.pup         : getattr(self, line[0])(*line)


    def BLNK(self, *line): pass
    def ELOB(self, *line): pass
    def GCAT(self, *line): pass
    def GLCZ(self, *line): pass
    def MOFF(self, *line): pass
    def OBSC(self, *line): pass
    def PRIM(self, *line): pass
    def RSCE(self, *line): pass
    def RWRE(self, *line): pass
    def SQOB(self, *line): pass
    def TRAC(self, *line): pass
    def VERS(self, *line): pass
    def ZVAN(self, *line): pass 
    def ZVCX(self, *line): pass
    def ZVCY(self, *line): pass
    def ZVDX(self, *line): pass
    def ZVDY(self, *line): pass

    def UNIT(self, lens_unit, src_prefix, src_unit, anal_prefix, anal_unit, *line): pass
    def ENVD(self, temp, pres, *line):	{ my set temperature $temp; my set presure $pres }
    def ENPD(self, size, *line):	{ my set pupilDiameter $size }


    def NAME { args } { set     Name $args }
    def NOTE { args } { lappend Notes $args }

    def  SURF { id } { set Id $id;  set comment {} }

    # Common surface definition commands
    #
    def TYPE { type args } { 

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

    def Process-Type { type comm args } {

	if type eq "USERSURF":
	    set args [lassign $args type]
	}
	set args {}

	set type ZMXSurfaceMap[type.tolower()]			; # Map Zemax surface type in to acorn.

	set surftype $type

	 Get the surface traverse and infos functions
	
	#$current set $surf type     $type
	#$current set $surf traverse $::acorn::SurfaceTypes($type)
	#$current set $surf infos    $::acorn::SurfaceInfos($type)

	if { [$current get $surf infos] != -1 } {
	    lassign [::acorn::infos 1 [$current get $surf infos]] dparams dvalues
	    lassign [::acorn::infos 2 [$current get $surf infos]] sparams svalues
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

	$current set $surf {*}[mappair $parmap [dict merge $basedef $default [join $args] [list name $Id type $surftype comment $comm]]]

	self.__dict__["s" + surf] = ...
	if { $comment ne {} } {
	    self.__dict__["s" + deletespace(self.name)] = 

	}

	$surftype [self] $Id
    }
    def CURV(self, curv, *line):
	 if $grouptype ne "non-sequential":
	     if $curv:
		 surf.R = 1.0/curv

    def CONI(self, conic, *line):
	     surf.K = conic

    def COMM(self, *line): self.state.comment = " ".join(*line)

    def PARM { n value } {
	 if { $grouptype ne "non-sequential" } {
	     try { my [$current get $surf name] set $::acorn::ZMXParmMap($surftype,$n) $value
	     } on error message {
		 if { $debug } { puts stderr "PARM $surftype $n $value : $message" }
	     }
	 } else {
	     $current set $surf p$n $value
	 }
    }
    def DISZ(self, thick, *line):
	 if self.state.grouptype == "non-sequential": self.surf.param.thickness = thick
 
    def DIAM(self, diam, *line): self.param.semi = diam	; # This is Zemax computed semi-diameter, not the aperture size.

    def SQAP(self, w, h, *line):
	self.surf.aper_type = "rectangle"
     	self.surf.aper_min  = w 
     	self.surf.aper_max  = h 

    def ELAP(self, w, h, *line):
	self.surf.aper_type = "eliptical"
     	self.surf.aper_min  = w/2.0
     	self.surf.aper_max  = h/2.0 
 
    def CLAP(self, rad, *line):
	self.surf.aper_type = "circular"
     	self.surf.aper_max  = rad

    def FLAP(self, n, rad, *line):
	self.surf.aper_type = "circular"
     	self.aper_max  = rad

    def OBSC(self, n, rad, *line):
	self.surf.aper_type = "obstruction"
     	self.surf.aper_min  = rad

    def OBDC(self, x y, *line):	 					# aperture decenter 
     	self.surf.aper_xoff = x
     	self.surf.aper_yoff = y
    }

    def GLAS(self, name, *line): 
	 self.surf.glass = GlassIndex(name)

     # NonSequential surface commands
     #
     def NSOH { type args } {
	switch $type {
	 default {
	    if { $nonseq == 0 } {
		set nsoexit [$current get 0 p0 p1 p2 p3 p4 p5 p6]
		$current set 0 p0 0 p1 0 p2 0 p3 0 p4 0 p5 0 p6 0
	    }

	    $current length = $nonseq

	    my SURF $nonseqid-[incr nonseq]

	    lassign $args a b comment

	    my Process-Type $type $comment

	    my [$current get $surf name] set thickness [lindex $nsoexit 3]
	 }
	}
    }
    def NSOA { n aperture } {

	if { $aperture eq  {}  } {
	    $current set $surf aper_type  circular
	    return
	}

	$current set $surf aper_type  UDA
	$current set $surf aper_param [string map { {"} {} } $aperture]

	lappend objects [set aper [::acorn::Aperture [$current get $surf aper_type] [$current get $surf aper_param]]]
	$current set $surf aperture [$aper polygon]
    }
    def NSCS { args } {}
    def NSOD { n value a b c d e f } {
	 try { my [$current get $surf name] set $::acorn::ZMXNSODMap($surftype,$n) $value 
	 } on error message {
	     my [$current get $surf type] $n $value $a $b $c $d $e $f
	 }
    }
    def NSOP { dx dy dz rx ry rz args } {
	 my [$current get $surf name] set x $dx y $dy z $dz rx $rx ry $ry rz $rz

	 if { [lindex $args 0] eq "MIRROR" } {
	     my [$current get $surf name] set n -1 
	 }
    }
    def NSCD(self, *line): pass
    def NSOO(self, *line): pass
    def NSOQ(self, *line): pass
    def NSOS(self, *line): pass
    def NSOU(self, *line): pass
    def NSOV(self, *line): pass
    def NSOW(self, *line): pass


    def EFFL(self, *line): pass
    def COAT(self, *line): pass
    def COFN(self, *line): pass
    def CONF(self, *line): pass
    def DMFS(self, *line): pass
    def FLOA { args } { set float 1 }
    def FTYP { a b nfield nwave args } { my set nfield $nfield; my set nwave $nwave }
    def FWGT(self, *line): pass
    def FWGN(self, *line): pass
    def GFAC(self, *line): pass
    def GLRS(self, *line): pass
    def GSTD(self, *line): pass
    def HIDE(self, *line): pass
    def MAZH(self, *line): pass
    def MIRR(self, *line): pass
    def MODE(self, *line): pass

    def PFIL(self, *line): pass
    def PICB(self, *line): pass
    def POLS(self, *line): pass
    def POPS(self, *line): pass
    def PUSH(self, *line): pass
    def PUPD(self, *line): pass
    def PWAV(self, *line): pass
    def RAIM(self, *line): pass
    def ROPD(self, *line): pass
    def SCOL(self, *line): pass
    def SDMA(self, *line): pass
    def SLAB(self, *line): pass
    def STOP { args } { my set stop $Id }
    def TOL (self, *line): pass
    def TOLE(self, *line): pass
    def VANN(self, *line): pass
    def VCXN(self, *line): pass
    def VCYN(self, *line): pass
    def VDSZ(self, *line): pass
    def VDXN(self, *line): pass
    def VDYN(self, *line): pass
    def XDAT(self, *line): pass
    def XFLD(self, *line): pass
    def YFLD(self, *line): pass

    def WAVL { wave }          { my set wavelength current [expr $wave*10000] }
    def WAVM { n wave weight } {
				    my set wavelength $n wave    [expr $wave*10000]
				    my set wavelength $n weight  $weight
    }
    def WWGT { weight } {	    my set wavelength weight  $weight }
    def WAVN { args } {
	foreach wave   $args {	    my set wavelength [incr n] wave    [expr $wave*10000] }
    }
    def WWGN { args } {
	foreach weight $args {      my set wavelength [incr n] weight  $weight }
    }

    def XFLN { args } { set fieldx $args }
    def YFLN { args } { set fieldy $args }

    # Pickups
    #
    def PZUP {       from scale offset { column 0 } } { append pup "my xPZUP $Id [expr int($from)] $scale $offset $column\n" }
    def PPAR { param from scale offset { column 0 } } { append pup "my xPPAR $Id [expr int($from)] $scale $offset $column $param\n" }

    def xPZUP { surf from scale offset column } { my $surf thickness set [expr [my $from get thickness]*$scale] }
    def xPPAR { surf from scale offset column param } {
	if { $from <= 0 } { 
	    
	    # Try a chief ray solve
	    #
	    lassign [my get field 1] x fx y fy			; # Get the current field angles
	    $aray set px 0 py 0 pz 0 vignetted 0		; # Set up aray.
	    $aray angles : $fx $fy

	    my  $surf set $acorn::ZMXParmMap([my $surf get type],$param) 0.0			; # Set the parameter to be solved to 0

	    [self] trace $aray [list 1 $surf] [my get wavelength current] 			; # Trace to the surface.

	    set value [$aray get p$acorn::ZMXParmMap([my $surf get type],$param)]		; # Copy the parameter from the ray to the surface.
	} else {
	    set value [expr { [my $from get $acorn::ZMXParmMap([my $surf get type],$param)]*$scale+$offset }]
	}

	my $surf set $acorn::ZMXParmMap([my $surf get type],$param) $value
    }


    # Multi Configuration Editor
    #
    def MNUM { n { curr 1 } } { set mce_current $curr }				; # Multi Configure Number of configs

    def WAVE { wave  config args } { append mce($config) "my xWAVE $wave  $args\n" }	; # Set Wavelength
    def IGNR { surf  config args } { append mce($config) "my xIGNR $surf  $args\n" }	; # Ignore surface
    def PRAM { surf  config args } { append mce($config) "my xPRAM $surf  $args\n" }	; # Set Parameter
    def XFIE { field config args } { append mce($config) "my xXFIE $field $args\n" }	; # Set X Field
    def YFIE { field config args } { append mce($config) "my xYFIE $field $args\n" }
    def THIC { surf  config args } { append mce($config) "my xTHIC $surf  $args\n" }

    def xNOP(self, *line): pass

    def xIGNR { surf  value args }         { my $surf set enable [expr !int($value)] }
    def xPRAM { surf  value x param args } { my $surf set $acorn::ZMXParmMap([my $surf get type],$param) $value }
    def xTHIC { surf  value args }         { my $surf set thickness                                      $value }
    def xXFIE { field value args } 	     { 	my set field $field x $value }
    def xYFIE { field value args } 	     { 	my set field $field y $value }
    def xWAVE { wave  value args } 	     {  my set wavelength $wave wave $value }

