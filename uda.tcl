# Zemax UDA file.
#
# 
# ARC cx cy angle n
# BRK
# CIR cx cy radius n
# ELI cx cy rx ry angle n
# LIN x y n
# x y
# POL cx cy radius n angle
# REC cx cy w h angle
# ! Comment

lappend auto_path lib

source tcloo.tcl

package require arec

arec::typedef ::acorn::Polygon {
    double	x;
    double	y;
    double	z;
}

# The UDA file is read in and reduced to a polygon definition
#

oo::class create UDA {
    variable polygon inpath
    accessor polygon

    constructor { type args } {
	procs ARC BRK CIR ELI LIN POL REC !
	namespace unknown { my unknown }

	set polygon [acorn::Polygon new [namespace current]::polygon]
	set inpath  -1

	switch $type {
	    source { source [lindex $args 0] } 
	    string { eval {*}$args }
	}
	BRK
    }

    method rpoly { cx cy radius n } {
	BRK
	foreach { x y } [rp cx cy radius n] {
	    $polygon [$polygon length] set x $x y $y
	}
    }

    method CIR { cx cy r { n 64 } } 		{ rpoly $cx $cy $r $n }
    method ELI { cx cy rx ry rot { n {} }  }	{}
    method REC { cx cy w h rot } {}
    method POL { cx cy radius n rot } {}

    method ARC { cx cy angle { n {} } } {
	if { $inpath == -1 } { set inpath [$polygon length] }
    }
    method LIN { x y { n {} } } 		{
	if { $inpath == -1 } { set inpath [$polygon length] }

	$polygon [$polygon length] set x $x y $y 
    }
    method BRK {} 				{
	if { $inpath != -1 } { LIN {*}[lindex [$polygon $inpath get x y] 0] }

	$polygon [$polygon length]  set x 0  y 0 
    }
    method !   { args } {}

    method unknown { args } {
	set args [string map { , {} } $args]

	switch -regexp -- [lindex $args 0] {
	    [-+0-9]* {
		if { [lindex $args 0] == 0 && [lindex $args 1] == 0 } {
		    BRK
		} else {
		    LIN {*}$args
		}
	    }
	}
    }
}

#UDA create m1aper source m1aper1e.uda
#puts [[m1aper polygon] 1 end get]

