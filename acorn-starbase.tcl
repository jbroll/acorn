
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

