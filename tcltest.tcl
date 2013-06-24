
set MatchTol 0.000001


try {
    package present tcltest

    ::tcltest::customMatch tol TolMatch
    proc TolMatch { a b } {
	foreach va $a vb $b {
	    if { abs($va-$vb) > $::MatchTol } { return 0 }
	}
	return 1
    }
} on error message {
    if { $message ne "package tcltest is not present" } {
	error $message
    }
}

