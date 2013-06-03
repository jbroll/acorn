

proc starbase2ray { table { rays {} } } {
    upvar $table T

    if { $rays eq "" } {
	set rays [::acorn::Rays create _rays[incr ::acorn::RAYS] 0]
    }

    starbase_foreachrow T -colvars row {
	$rays [expr $row-1] set px $x py $y pz $z kx $l ky $m kz $n
    }

    set rays
}

proc starbase_raycompare { rays file zoff { big 10000 } } {
    starbase_read ARay $file

    set bigs {}

    set sumx 0
    set sumy 0
    set sumz 0
    set count 0

    starbase_foreachrow ARay -colvars row {
	if { !$v && ![$rays [expr $row-1] get vignetted] } {
	    lassign [$rays [expr $row-1] get px py pz] px py pz

	    if { abs($x - $px) > $big || abs($y - $py) > $big } { 
		lappend bigs $row [expr { $x - $px }] [expr { $y - $py }]
		continue
	    }

	    set pz [expr { $pz-$zoff }]

	    set sumx [expr { $sumx + ($x - $px)*($x - $px) } ]
	    set sumy [expr { $sumy + ($y - $py)*($y - $py) } ]
	    set sumz [expr { $sumz + ($z - $pz)*($z - $pz) } ]

	    incr count
	}
	

    }

    list $sumx $sumy $sumz $count $bigs
}

