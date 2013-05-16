
proc starbase2ray { table { rays {} } } {
    upvar $table T

    if { $rays eq "" } {
	set rays [::acorn::Rays create rays[incr ::acorn::RAYS] 0]
    }

    starbase_foreachrow T -colvars row {
	$rays [expr $row-1] set px $x py $y pz $z kx $l ky $m kz $n
    }

    set rays
}

proc starbase_raycompare { rays file zoff } {
    starbase_read ARay $file

    set sumx 0
    set sumy 0
    set sumz 0
    starbase_foreachrow ARay -colvars row {
	lassign [$rays [expr $row-1] get px py pz] px py pz

	set pz [expr $pz-$zoff]

	#puts "$px $py $pz		$x $y $z"

	set sumx [expr { $sumx + ($x - $px)*($x - $px) } ]
	set sumy [expr { $sumy + ($y - $py)*($y - $py) } ]
	set sumz [expr { $sumz + ($z - $pz)*($z - $pz) } ]
    }

    unset ARay

    list $sumx $sumy $sumz
}

