
proc acorn::mkrays { name args } {
    if { $name eq "-" } { set name rays[incr ::acorn::RAYS] }
    set pz 0
    set args [dict merge { type acorn::Rays circle 0 nx 11 ny 11 x0 -5 x1 5 y0 -5 y1 5 xi - yi - } $args]


    dict with args {
	if { [info commands $name] eq {} } { $type create $name 0 }

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

		$name set end+1 px $x py $y pz $pz kx 0.0 ky 0.0 kz 1.0 vignetted 0
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


proc acorn::rays-rms { rays { v 0 } { cx 0 } { cy 0 } } {
    set x 0
    set y 0
    set r 0
    set n 0

    rays-foreach $rays {
	if { $px != $px || $py != $py || $pz != $pz } { continue }
	if { $v && $vignetted                       } { continue }

	set x [expr { $x + ($px-$cx) * ($px-$cx) }]
	set y [expr { $y + ($py-$cy) * ($py-$cy) }]

	set rr [expr { sqrt(($px-$cx) * ($px-$cx) + ($py-$cy) * ($py-$cy)) }]

	set r [expr { $r + $rr*$rr }]

	incr n
    }

    if { $n } {
	list [expr { $x != 0.0 ? sqrt($x/$n) : 0.0 }]  [expr { $y ? sqrt($y/$n) : 0.0 }] [expr { $r ? sqrt($r/$n) : 0.0 }] $n
    } else {
	list 0.0 0.0 0.0 0
    }
}

proc acorn::rays-stat { rays { v 0 } } {
    set cx 0
    set cy 0
    set n  0

    rays-foreach $rays {
	if { $px != $px || $py != $py || $pz != $pz } { continue }
	if { $v && $vignetted                       } { continue }

	set cx [expr { $cx + $px }]
	set cy [expr { $cy + $py }]

	incr n
    }

    if { $n } {
	set cx [expr { $cx/$n }]
	set cy [expr { $cy/$n }]
    }

    list $cx $cy {*}[rays-rms $rays $v $cx $cy]
}

proc rays-foreach { rays body } {
    set cmd [subst { foreach _i \[iota 0 [$rays length]-1] {
	lassign \[$rays get \$_i] [$rays type name] 
	$body
    }}]

    uplevel 1 $cmd
}

proc rays-select { rays expr { cols {} } } {
    if { $cols eq "" } {
	set cols [$rays type names]
    }

    uplevel [subst {
	set _1 {}
	rays-foreach $rays { if { !($expr) } { continue } ; lappend _1 $[join $cols " $"] }
	return \$_1
    }]
}
