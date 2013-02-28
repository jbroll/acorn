
    proc mappair { map params } {
	set reply {}
	foreach { name value } $params { lappend reply [dict get $map $name] $value }
	set reply
    }
    proc maplist { map params } {
	set reply {}
	foreach name $params { lappend reply [dict get $map $name] }
	set reply
    }




oo::class create ::acorn::Model {
    variable type current surfaces default basedef basemap
    accessor type current surfaces default

    constructor { args } {
	procs surface coordinate-break surface-group surface-group-non-sequential

	set type sequential
	set basedef { type simple n 1.00 }
	set default {}
	set basemap { type type R R K K n n thickness thickness aper_type aper_type aper_param aper_param x x y y z z rx rx ry ry rz rz }


	set body [lindex $args end]
	set args  [lrange $args 0 end-1]

	foreach { name value } [dict merge { type sequential } $args] { set $name $value }

	set current [::acorn::Surfs create [namespace current]::surfs[incr [namespace current]::SURFS] 0]

	eval $body

	if { [$current length] } { lappend surfaces $type $current }

	set current Surface-Add-Error
    }
    method coordinate-break { name args } { surface name [list {*}[join $args] type coordbk] }


    method surfset1 { obj surf parmap cmd args } {
	switch $cmd {
	    set { $obj $surf $cmd {*}[mappair $parmap $args] }
	    get { $obj $surf $cmd {*}[maplist $parmap $args] }
	}
    }


    method surface { name args } {
	set i [$current length]

	set params [dict merge $basedef $default [join $args]]
	set type   [dict get   $params type]

	$current $i set traverse $::acorn::SurfaceTypes($type)		; # Get the surface traverse and infos functions
	$current $i set infos    $::acorn::SurfaceInfos($type)

	lassign [::acorn::infos 1 [$current $i get infos]] params values

	set k 0
	set parmap {}
	foreach param $params { lappend parmap $param p$k; incr k }	; # Query for and map parameters

	lassign [::acorn::infos 2 [$current $i get infos]] params values

	set k 0
	foreach param $params { lappend parmap $param s$k; incr k }	; # Query for and map strings

	set parmap [dict merge $basemap $parmap]

	$current $i set {*}[dict merge $basedef $default [mappair $parmap [join $args]] [list name $name]]
	$current $i set aperture [::acorn::Aperture [$current get aper_type] [$current get aper_param]]

	if { $name ne "." } {
	    ::oo::objdefine [self] [list forward $name [self] surfset1 $current $i $parmap]
	    ::oo::objdefine [self] [list export $name]
	}
    }
    method surface-group { name args } {
	set savedefault $default
	set savetype    $type

	if { [$current length] } {
	    lappend surfaces $type $current
	    set current [::acorn::Surfs create [namespace current]::surfs[incr [namespace current]::SURFS] 0]
	}

	set body [lindex $args end]
	set args  [lrange $args 0 end-1]

	foreach { name value } [dict merge { type sequential } $args] { set $name $value }

	eval $body

	if { [$current length] } { lappend surfaces $type $current }
	set current [::acorn::Surfs create [namespace current]::surfs[incr [namespace current]::SURFS] 0]

	set default $savedefault
	set type    $savetype
    }
    method surface-group-non-sequential { name args } { tailcall surface-group $name type non-sequential {*}$args }

    method trace { rays } {
	::acorn::SurfaceList create slist 0

	set i 0

	foreach { type surf } $surfaces {
	    slist $i set surf [$surf getptr] nsurf [$surf length] type [string equal $type non-sequential]

	    foreach j [iota 0 [$surf length]-1] {
		set aper [lindex [lindex [$surf $j get aperture] 0] 0]

		if { $aper ne {} } {
		    $surf $j set aper_data [$aper getptr]
		    $surf $j set aper_leng [$aper length]
		}
	    }
	    incr i
	}
	acorn::trace_rays 0 1 [slist getptr] [slist length] [$rays getptr] [$rays length]

	rename slist {}
    }

    method print {} {
	foreach { type surf } $surfaces {
	    puts "$type : $surf :"
	    puts "	[join [$surf 0 end get] "\n	"]"
	    puts \n
	}
    }
}

proc acorn::model { args } { tailcall acorn::Model create {*}$args }

proc acorn::mkrays { name args } {
    if { $name eq "-" } { set name rays[incr ::acorn::RAYS] }

    acorn::Rays create $name 0

    set args [dict merge { nx 11 ny 11 x0 -5 x1 5 y0 -5 y1 5 xi - yi - } $args]
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
		$name $i set px $x py $y pz 0 kx 0.0 ky 0.0 kz 1.0 vignetted 0
		incr i
	    }
	}
    }

    return $name
}

proc acorn::prays { rays } {
    puts "i	x	y	z	kz	ky	kz	v"
    puts "-	-	-	-	--	--	--	-"
    acorn::_prays [$rays getptr] [$rays length]
}


