
proc acorn::rays-rms { rays { v 0 } } {
    set x 0
    set y 0
    set z 0
    set r 0
    set n 0

    set l [$rays length]

    foreach i [iota 0 $l-1] {
	if { $x != $x || $y != $y || $z != $z } { continue }
	if { $x != $x || $y != $y || $z != $z } { continue }
	if { $v && [$rays $i get vignetted]   } { continue }

	set x [expr { $x + [$rays $i get px] * [$rays $i get px] }]
	set y [expr { $y + [$rays $i get py] * [$rays $i get py] }]
	set z [expr { $z + [$rays $i get pz] * [$rays $i get pz] }]
	set r [expr { $r + sqrt([$rays $i get px] * [$rays $i get px] + [$rays $i get py] * [$rays $i get py])}]

	incr n
    }

    list [expr { $x != 0.0 ? sqrt($x/$l) : 0.0 }]  [expr { $y ? sqrt($y/$l) : 0.0 }] [expr { $z ? sqrt($z/$l) : 0.0 }] [expr { $r ? sqrt($r/$l) : 0.0 }] $n
}

proc rays-foreach { rays body } {
    set cmd [subst { foreach _i \[iota 0 [$rays length]-1] {
	lassign \[$rays \$_i get] [$rays type names] 
	$body
    }}]

    uplevel 1 $cmd
}

proc rays-map { rays expr { cols {} } } {
    if { $cols eq "" } {
	set cols [$rays type names]
    }

    uplevel [subst {
	set _1 {}
	rays-foreach $rays { if { !($expr) } { continue } ; lappend _1 $[join $cols " $"] }
	return \$_1
    }]
}
