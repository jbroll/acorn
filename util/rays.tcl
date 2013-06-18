
proc acorn::rays-rms { rays } {
    set x 0
    set y 0
    set z 0
    set r 0

    set l [$rays length]

    foreach i [iota 0 $l-1] {
	set x [expr { $x + [$rays $i get px] }]
	set y [expr { $z + [$rays $i get py] }]
	set z [expr { $z + [$rays $i get pz] }]
	set r [expr { $r + sqrt([$rays $i get px] * [$rays $i get px] + [$rays $i get py] * [$rays $i get py])}]
    }

    list [expr { sqrt($x)/$l }]  [expr { sqrt($y/$l) }] [expr { sqrt($z/$l) }] [expr { sqrt($r/$l) }]
}
