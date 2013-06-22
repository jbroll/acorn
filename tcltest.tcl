
set MatchTol 0.000001

::tcltest::customMatch tol TolMatch
proc TolMatch { a b } {
    foreach va $a vb $b {
	if { abs($va-$vb) > $::MatchTol } { return 0 }
    }
    return 1
}
