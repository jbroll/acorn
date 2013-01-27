#!/Users/john/bin/tclkit8.6
#

lappend auto_path ./lib

package require arec
package require acorn

source tcloo.tcl


acorn::init

 RayType new  rays 1

oo::class create surface {
    variable s type
    accessor s type


    constructor { args } {
	set type sequential
	set s [SurfType new [namespace current]::surfs 1]

	$s set {*}[dict merge { type simple } $args [list name [string range [self] 2 end]]]

	$s set traverse $::acorn::SurfaceTypes([$s get type])
    }
}

oo::class create surface-group {
    variable s type
    accessor s type

    constructor { args } {
	set surfs [lindex $args end]
	set args  [lrange $args 0 end-1]

	foreach { name value } [dict merge { type sequential } $args] { set $name $value }

	set s [SurfType new [namespace current]::surfs [expr [llength $surfs]/2]]

	set i 0
	foreach { name params } $surfs {
	    $s $i set {*}[dict merge { type simple } $params [list name $name]]

	    $s $i set traverse $::acorn::SurfaceTypes([$s $i get type])

	    incr i
	}

	#puts [$s 0 end get]
    }
}

set fields { aper R K n thinkness }

surface       create  Start R 0 K 0 thickness 10000

surface-group create  P type non-sequential  {
    P1 { }
    P2 { }
    P3 { }
    P4 { }
    P5 { }
    P6 { }
    P7 { }
}

surface-group create  S type non-sequential {
    S1 { R 401 }
    S2 { R 402 }
    S3 { R 403 }
    S4 { R 404 }
    S5 { R 405 }
    S6 { R 406 }
    S7 { R 407 }
}

surface       create  Focus R 400

proc acorn::trace { rays args } {
    foreach group $args {
	switch [$group type] {
	        sequential { acorn::trace_seq 0 1 [[$group s] getptr] [[$group s] length] [rays getptr] [rays length] }
	    non-sequential { acorn::trace_nsq 0 1 [[$group s] getptr] [[$group s] length] [rays getptr] [rays length] }
	    default        { error "unknown surface type [$group type]" }
	}
    }
}

acorn::trace rays Start P S Focus

