
    proc dltpair { l1 l2 } {
	set reply {}
	foreach { name def } $l1 {
	    set val [dict get $l2 $name]

	    if { $def != $val } {
		lappend reply $name $val
	    }
	}

	set reply
    }
    proc mappair { map params } {
	set reply {}
	foreach { name value } $params {
	    try {
		lappend reply [dict get $map $name] $value
	    } on error message {
		puts "$message : $name"
	    }
	}
	set reply
    }
    proc revpair { map params } {
	foreach { name par } $map { lappend rev $par $name }

	set reply {}
	foreach { name value } $params { lappend reply [dict get $rev $name] $value }
	set reply
    }
    proc maplist { map params } {
	if { [llength $params] == 0 } { return [dict values $map] }

	set reply {}
	foreach name $params { lappend reply [dict get $map $name] }
	set reply
    }


oo::class create ::acorn::BaseModel {
    variable grouptype current surf surftype surfaces default basedef basemap basepar anonsurf surfdefs mce objects
    variable wavelength pupilDiameter

    accessor surfaces basepar

    constructor {} {
	set grouptype sequential
	set basedef { name {} type simple comment {} n 1.00 glass {} x 0.0 y 0.0 z 0.0 rx 0.0 ry 0.0 rz 0.0 thickness 0.0 aper_type {} aper_param {} aper_min 0.0 aper_max 0.0 enable 1 annot 0 }
	set default {}
	set basemap { 	name name type type comment comment glass glass
	    		aperture aperture aper_type aper_type aper_param aper_param aper_data aper_data aper_leng aper_leng 
			traverse traverse infos infos thickness thickness enable enable annot annot
	}
    	set basepar { x y z rx ry rz thickness aper_min aper_max n }
	foreach par $basepar i [iota 0 [llength $basepar]-1] { lappend basemap $par p$i }
	set anonsurf 0

	set surftype {}
	set current [::acorn::Surfs create [namespace current]::surfs[incr [namespace current]::SURFS] 0]
	set surf 0

	dict set wavelength current 5000
	dict set wavelength 1 	    { wave 5000 weight 1 }
    }
    destructor {
	foreach { type surf } $surfaces {
	    rename $surf {}
	}
	foreach object $objects { 
	    $object destroy
	}

    }

    method set { args } { dict set  parameters {*}$args }
    method get { args } { dict get $parameters {*}$args }

    method wavelength { op args } { 
	switch $op {
	    set { dict set  wavelength {*}$args }
	    get { dict get $wavelength {*}$args }
	}
    }

    method surfset1 { obj surf parmap cmd args } {
	switch $cmd {
	    length  { $obj $cmd $surf }
	    set     { $obj $cmd $surf {*}[mappair $parmap $args] }
	    setdict { $obj $cmd $surf {*}[mappair $parmap $args] }
	    setlist { $obj $cmd $surf {*}$args] }
	    get     { $obj $cmd $surf {*}[maplist $parmap $args] }
	    getdict { revpair $parmap [$obj $cmd $surf {*}[maplist $parmap $args]] }
	    getlist { $obj $cmd $surf {*}[maplist $parmap $args] }

	    params { lmap { name par } $parmap { set name } }
	    map    { return $parmap }
	}
    }

    method trace { { rays {} } { surfs {} } { wave current } { thread 0 } { xray 0 } } {		# Assemble the surfaces to be traced.
	set ok 0

	if { $rays  eq {} } {
	    set rays [acorn::mkrays - nx 200 ny 200 diameter $pupilDiameter intensity 10]
	    $rays angles : [my field get 1 x] [my field get 1 y]
	}

	if { $surfs eq {} || $surfs == -1 } { set ok 1 }

	lassign $surfs start end

	if { $xray ne 0 } { set xray [$xray getptr] }

	::acorn::SurfaceList create slist 0
	::acorn::ModelData   create mdata 1

	if { $wave eq "current" } {
	    set wave [my wavelength get current]
	}
	mdata set 0 z 0
	mdata set 0 n 1
	mdata set 0 w $wave

	set i 0

	foreach { type surf } $surfaces {

	    slist set $i surf [$surf getptr] nsurf [$surf length] type [string equal $type non-sequential]

	    foreach j [iota 0 [$surf length]-1] {
		if { !$ok && $start eq [$surf get $j name] } { set ok 1 }
		if { !$ok } { continue } 

		set aper [$surf get $j aperture]

		if { $aper ne {} && $aper ne "(null)" } {
		    $surf set $j aper_data [$aper getptr]
		    $surf set $j aper_leng [$aper length]
		}

		if { [$surf get $j glass] ne {} && [$surf get $j glass] ne "(null)" } {
		    if { [$surf get $j glass_ptr] == -1 } {
			my [$surf get $j name] set n -1
		    } else {
			my [$surf get $j name] set n [acorn::glass_indx [$surf get $j glass_ptr] $wave]
		    }
		}


		if { $end eq [$surf get $j name] } {
		    slist set $i nsurf [expr $j+1]
		    break
		}
	    }
	    if { $j < [$surf length]-1 } { break }

	    incr i
	}

puts $thread
	acorn::trace_rays [mdata getptr] [slist getptr] [slist length] [$rays getptr] [$rays length] [$rays size] $thread $xray

	rename slist {}

	return $rays
    }

    method image { image args } {
	my bin [my trace] 
    }

    method trace1 { surface rays { z 0 } } {
	::acorn::SurfaceList create slist 0
	::acorn::ModelData   create mdata 1

	mdata set n 1
	mdata set z $z
	mdata set w 5000

	foreach { type surf } $surfaces {			# Find the surface to trace
	    foreach j [iota 0 [$surf length]-1] {
		if { [$surf $j get name] eq $surface } { break }
	    }
	    if { [$surf $j get name] eq $surface } { break }
	}

	set aper [lindex [lindex [$surf $j get aperture] 0] 0]

	if { $aper ne {} && $aper ne "(null)" } {
	    $surf $j set aper_data [$aper getptr]
	    $surf $j set aper_leng [$aper length]
	}
	slist 0 set surf [expr { [$surf getptr]+[acorn::Surfs size]*$j }] nsurf 1 type 0


	acorn::trace_rays [mdata getptr] [slist getptr] [slist length] [$rays getptr] [$rays length] [$rays size] 0 0
    }

    method print { { pipe {} } { out stdout } } {
	if { $pipe eq ">" } { set out [open $out w] }

	puts $out "acorn::model [self] \{"

	set tab "	"
	foreach { type surf } $surfaces {
	    if { $type eq "non-sequential" } { puts $out "${tab}surface-group-non-sequential X \{" ; set tab "		" }

	    foreach i [iota 0 [$surf length]-1] {

		#puts "	[[self] [$surf get $i name] getdict name type] : [[self] [$surf get $i name] get type]"

		set stype [[self] [$surf get $i name] get type]
		set values [[self] [$surf get $i name] getdict {*}[map { name value } $surfdefs($stype,pmap) { set name }]]

		puts $out "${tab}surface [$surf get $i name] \{ [dict remove [dltpair $surfdefs($stype,pdef) $values] name] \}"
	    }
	    set tab "        "
	    if { $type eq "non-sequential" } { puts $out "${tab}\}" }
	}
	puts $out "\}"

	if { $pipe eq ">" } { close $out }
    }

    method foreach-surface { value script } {
	set i 0

	foreach { type surf } $surfaces {
	    #slist $i set surf [$surf getptr] nsurf [$surf length] type [string equal $type non-sequential]

	    foreach j [iota 0 [$surf length]-1] {
		uplevel [list set $value [$surf $j get name]]
		uplevel $script
	    }
	    incr i
	}
    }
}

oo::class create ::acorn::Model {
    superclass ::acorn::BaseModel

    variable grouptype current surfaces default basedef basemap basepar anonsurf surfdefs objects wavelength
    accessor grouptype current surfaces default basepar

    constructor { args } {
	next 

	procs surface coordbrk surface-group surface-group-non-sequential


	set body [lindex $args end]
	set args [lrange $args 0 end-1]

	foreach { name value } [dict merge { grouptype sequential } $args] { set $name $value }

	set current [::acorn::Surfs create [namespace current]::surfs[incr [namespace current]::SURFS] 0]

	eval $body

	if { [$current length] } { lappend surfaces $grouptype $current 
	} else {
	    rename $current {}
	}

	set current Surface-Add-Error
    }
    method coordbrk { name args } { my surface name [list {*}[join $args] type coordbrk] }

    method surface { name args } {

	set i [$current length]

	set params [dict merge $basedef $default [join $args]]
	set type   [dict get   $params type]

	$current set $i traverse $::acorn::SurfaceTypes($type)		; # Get the surface traverse and infos functions
	$current set $i infos    $::acorn::SurfaceInfos($type)

	if { [$current get $i infos] != -1 } {
	    lassign [::acorn::infos 1 [$current get $i infos]] dparams dvalues
	    lassign [::acorn::infos 2 [$current get $i infos]] sparams svalues
	} else {
	    set dparams {}
	    set dvalues {}
	    set sparams {}
	    set svalues {}
	}

	if { ![info exists surfdefs($type,pdef)] } { set surfdefs($type,pdef) [dict merge $basedef [zip $dparams $dvalues] [zip $sparams $svalues]] }

	set k [llength $basepar]
	set parmap {}
	foreach param $dparams { lappend parmap $param p$k; incr k }	; # Query for and map parameters
	set k 0
	foreach param $sparams { lappend parmap $param s$k; incr k }	; # Query for and map strings

	set parmap [dict merge $basemap $parmap]
	if { ![info exists surfdefs($type,pmap)] } { set surfdefs($type,pmap) $parmap }

	$current set $i {*}[mappair $parmap [dict merge $surfdefs($type,pdef) $default [join $args] [list name $name]]]

	lappend objects {*}[set aper [::acorn::Aperture [$current get $i aper_type] [$current get $i aper_param]]]
	if { $aper ne {} } { $current set $i aperture [$aper polygon] }

	$current set $i glass_ptr [glass-lookup [$current get $i glass]]

	if { $name eq "." } {
	    set name anon[incr anonsurf]
	    $current set $i name $name
	}

	::oo::objdefine [self] [list forward $name [self] surfset1 $current $i $parmap]
	::oo::objdefine [self] [list export  $name]

	if { [$current get $i comment] ne {} } {
	    ::oo::objdefine [self] [list forward [$current get $i comment] [self] surfset1 $current $i $parmap]
	    ::oo::objdefine [self] [list export  [$current get $i comment]]
	}
    }
    method surface-group { name args } {
	set savedefault $default
	set savetype    $grouptype

	if { [$current length] } {
	    lappend surfaces $grouptype $current
	    set current [::acorn::Surfs create [namespace current]::surfs[incr [namespace current]::SURFS] 0]
	}

	set body [lindex $args end]
	set args  [lrange $args 0 end-1]

	foreach { name value } [dict merge { grouptype sequential } $args] { set $name $value }

	eval $body

	if { [$current length] } { lappend surfaces $grouptype $current }
	set current [::acorn::Surfs create [namespace current]::surfs[incr [namespace current]::SURFS] 0]

	set default $savedefault
	set grouptype    $savetype
    }
    method surface-group-non-sequential { name args } { tailcall surface-group $name grouptype non-sequential {*}$args }

}

proc acorn::model { args } { tailcall acorn::Model create {*}$args }

