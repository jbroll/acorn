
proc K { x y } { set x }

proc +   { x y } { expr $x + $y }
proc *   { x y } { expr $x * $y }
proc sum { lst } { foldl + 0 $lst }
proc sqr { x   } { expr $x * $x }
proc max { x y } { expr $x > $y ? $x : $y }

proc jot { n { fr - } { to - } { by - }  } {

    if {  $n eq "-" } { set  n 10 } 
    if { $fr eq "-" } { set fr  0 }
    if { $to eq "-" } { set to [expr { $fr+$to*$by }] }
    if { $by eq "-" } { set by [expr { ($to-$fr)/($n-1) }]  }

    iota $fr $to $by
}


proc iota { fr { to {} } { in 1 } } {
    if { $to eq {} } {
	set to $fr
	set fr 0
    }
    set fr [expr $fr]
    set to [expr $to]

    for { set res {} } { $fr <= $to } { incr fr $in } {lappend res $fr } 
    set res
}

proc map { args } {
    uplevel [subst {
	set _[info frame] {}
	foreach {*}[list [lrange $args 0 end-1]] { lappend _[info frame] \[[lindex $args end]] }
	set _[info frame]
    }]
}

proc pick {func list} {
    set ret [list]
    foreach item $list {
	if {[eval $func [list $item]]} {
	    lappend ret $item
	}
    }
    return $ret
}

proc foldl {func res list} {
    foreach item $list { set res [eval $func [list $res $item]] }
    set res
}

proc foldr {func res list} {
    for {set i [llength $list]} {$i > 0} {incr i -1} {
	set res [eval $func [list [lindex $list [expr {$i-1}]] $res]]
    }
    set res
}

proc lremove { list value } {
    set indx [lsearch $list $value]

    lreplace $list $indx $indx
}

proc shuffle list {		# http://wiki.tcl.tk/941 shuffle10a
    set len [llength $list]
    while {$len} {
	set n [expr {int($len*rand())}]
	set tmp [lindex $list $n]
	lset list $n [lindex $list [incr len -1]]
	lset list $len $tmp
    }
    return $list
}

proc memo { proc } {
    rename $proc _$proc

    proc $proc { args } [subst -nocommands { 
	if { [info exists ::_${proc}(\$args)] } { return [set ::_${proc}(\$args)] }

	set ::_${proc}(\$args) [_${proc} {*}\$args]
    }]
}

memo iota
