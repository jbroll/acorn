
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
	foreach { name value } [join $params] { lappend reply [dict get $rev $name] $value }
	set reply
    }
    proc maplist { map params } {
	if { [llength $params] == 0 } { return [dict values $map] }

	set reply {}
	foreach name $params { lappend reply [dict get $map $name] }
	set reply
    }


oo::class create ::acorn::BaseModel {
    variable grouptype current surf surftype surfaces default basedef basemap basepar anonsurf surfdefs mce 
    accessor surfaces basepar

    constructor {} {
	set grouptype sequential
	set basedef { name {} type simple comment {} n 1.00 glass {} x 0.0 y 0.0 z 0.0 rx 0.0 ry 0.0 rz 0.0 thickness 0.0 aper_type {} aper_param {} aper_min 0.0 aper_max 0.0 }
	set default {}
	set basemap { 	name name type type comment comment glass glass
	    		aperture aperture aper_type aper_type aper_param aper_param aper_data aper_data aper_leng aper_leng 
			traverse traverse infos infos thickness thickness 
	}
    	set basepar { x y z rx ry rz thickness aper_min aper_max n }
	foreach par $basepar i [iota 0 [llength $basepar]-1] { lappend basemap $par p$i }
	set anonsurf 0

	set surfdefs(coordbrk,pdef) {}

	set surftype {}
	set current [::acorn::Surfs create [namespace current]::surfs[incr [namespace current]::SURFS] 0]
	set surf 0
    }
    method surfset1 { obj surf parmap cmd args } {
	switch $cmd {
	    length  { $obj $surf $cmd }
	    set     { $obj $surf $cmd {*}[mappair $parmap $args] }
	    setdict { $obj $surf $cmd {*}[mappair $parmap $args] }
	    setlist { $obj $surf $cmd {*}$args] }
	    get     { $obj $surf $cmd {*}[maplist $parmap $args] }
	    getdict { revpair $parmap [$obj $surf $cmd {*}[maplist $parmap $args]] }
	    getlist { $obj $surf $cmd {*}[maplist $parmap $args] }

	    params { lmap { name par } $parmap { set name } }
	    map    { return $parmap }
	}
    }

    method trace { rays { surfs { 0 end } } { wave 5000 } { thread 0 } } {		# Assemble the surfaces to be traced.
	::acorn::SurfaceList create slist 0

	lassign $surfs start end
	lassign $start s0 s1
	lassign $end   e0 e1

	set i 0
	foreach { type surf } $surfaces {
	    if { $s0 > $i } { continue }

	    slist $i set surf [$surf getptr] nsurf [$surf length] type [string equal $type non-sequential]

	    foreach j [iota 0 [$surf length]-1] {
		#if { $s1 > $i } { continue }

		set s1 0

		set aper [lindex [lindex [$surf $j get aperture] 0] 0]

		if { $aper ne {} } {
		    $surf $j set aper_data [$aper getptr]
		    $surf $j set aper_leng [$aper length]
		}

		if { [$surf $j get glass] ne {} } {
		    if { [$surf $j get glass_ptr] == -1 } {
			my [$surf $j get name] set n -1
		    } else {
			my [$surf $j get name] set n [acorn::glass_indx [$surf $j get glass_ptr] $wave]
		    }
		}

		#if { $i == $e0 && $j == $e1 } { break }
	    }
	    #if { $i == $e0 && $j == $e1 } { break }
	    incr i
	}

	acorn::trace_rays 0 1 [slist getptr] [slist length] [$rays getptr] [$rays length] [$rays size] $thread

	rename slist {}
    }

    method trace1 { surface rays { z 0 } } {
	::acorn::SurfaceList create slist 0

	foreach { type surf } $surfaces {			# Find the surface to trace
	    foreach j [iota 0 [$surf length]-1] {
		if { [$surf $j get name] eq $surface } { break }
	    }
	    if { [$surf $j get name] eq $surface } { break }
	}

	set aper [lindex [lindex [$surf $j get aperture] 0] 0]

	if { $aper ne {} } {
	    $surf $j set aper_data [$aper getptr]
	    $surf $j set aper_leng [$aper length]
	}
	slist 0 set surf [expr { [$surf getptr]+[acorn::Surfs size]*$j }] nsurf 1 type 0


	acorn::trace_rays $z 1 [slist getptr] [slist length] [$rays getptr] [$rays length] [$rays size] 0
    }

    method print {} {
	puts "acorn::model [self] \{"

	set tab "	"
	foreach { type surf } $surfaces {
	    if { $type eq "non-sequential" } { puts "${tab}surface-group-non-sequential X \{" ; set tab "		" }
	    foreach i [iota 0 [$surf length]-1] {
		#puts "	[[self] [$surf $i get name] getdict name type] : [[self] [$surf $i get name] get type]"
		set stype [[self] [$surf $i get name] get type]
		set values [[self] [$surf $i get name] getdict {*}[map { name value } $surfdefs($stype,pmap) { set name }]]
		puts "${tab}surface [$surf $i get name] \{ [dict remove [dltpair $surfdefs($stype,pdef) $values] name] \}"
	    }
	    set tab "        "
	    if { $type eq "non-sequential" } { puts "${tab}\}" }
	}
	puts "\}"
    }
}

oo::class create ::acorn::Model {
    superclass ::acorn::BaseModel

    variable grouptype current surfaces default basedef basemap basepar anonsurf surfdefs
    accessor grouptype current surfaces default basepar

    constructor { args } {
	next 

	procs surface coordinate-break surface-group surface-group-non-sequential


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
    method coordinate-break { name args } { surface name [list {*}[join $args] grouptype coordbk] }

    method surface { name args } {

	set i [$current length]

	set params [dict merge $basedef $default [join $args]]
	set type   [dict get   $params type]

	$current $i set traverse $::acorn::SurfaceTypes($type)		; # Get the surface traverse and infos functions
	$current $i set infos    $::acorn::SurfaceInfos($type)

	if { [$current $i get infos] != -1 } {
	    lassign [::acorn::infos 1 [$current $i get infos]] dparams dvalues
	    lassign [::acorn::infos 2 [$current $i get infos]] sparams svalues
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

	$current $i set {*}[mappair $parmap [dict merge $surfdefs($type,pdef) $default [join $args] [list name $name]]]
	$current $i set aperture  [::acorn::Aperture [lindex [$current $i get aper_type] 0] [$current $i get aper_param]]

	$current $i set glass_ptr [glass-lookup [$current $i get glass]]

	if { $name eq "." } {
	    set name anon[incr anonsurf]
	    $current $i set name $name
	}

	::oo::objdefine [self] [list forward $name [self] surfset1 $current $i $parmap]
	::oo::objdefine [self] [list export  $name]

	if { [$current $i get comment] ne {} } {
	    ::oo::objdefine [self] [list forward [$current $i get comment] [self] surfset1 $current $i $parmap]
	    ::oo::objdefine [self] [list export  [$current $i get comment]]
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

proc acorn::model { args } { tailcall acorn::Model create {*}$args }

proc acorn::mkrays { name args } {
    if { $name eq "-" } { set name rays[incr ::acorn::RAYS] }
    set pz 0

    if { [info commands $name] eq {} } { acorn::Rays create $name 0 }

    set args [dict merge { circle 0 nx 11 ny 11 x0 -5 x1 5 y0 -5 y1 5 xi - yi - } $args]
    dict with args {
	if { [info exists box] } {
	    set x0 [expr -$box]
	    set x1 [expr  $box]
	    set y0 [expr -$box]
	    set y1 [expr  $box]
	}

	set i 0
	foreach x [jot $nx $x0 $x1 $xi] {
	    foreach y [jot $ny $y0 $y1 $yi] {
		if { $circle && $x*$x+$y*$y > $x0*$x0+$y0+$y0 } { continue }

		$name [$name length] set px $x py $y pz $pz kx 0.0 ky 0.0 kz 1.0 vignetted 0
		incr i
	    }
	}
    }

    return $name
}

proc acorn::prays { rays { pipe {} } { out stdout } } {

    if { $pipe eq ">" } { set out [open $out w] }

    set i 1

    puts $out "id	x	y	z	l	m	n	v"
    puts $out "-	-	-	-	--	--	--	-"

    foreach row [$rays get px py pz kx ky kz vignetted] {
  	puts $out "$i	[join $row \t]"
	incr i
    }

    if { $pipe eq ">" } { close $out }
}


