#!/usr/bin/env tclkit8.6
#
lappend auto_path ./lib arec/lib

package require tcltest
package require acorn

source util/rays.tcl

acorn::ZMX create B source zmx-models/p8_110912af_072512ac.zmx
B config 1

switch $argv {
 print { B print }
 trace { 

    set w1 [B get wavelength 1 wave]
    set w2 [B get wavelength 2 wave]
    set w3 [B get wavelength 3 wave]

    set w $w1

    puts stderr "wave : $w1 $w2 $w3"

    acorn::mkrays rays nx 50 x0 -128 x1 128 ny 50 y0 -128 y1 128 circle 1

    puts Trace

    timer 1 start
    B trace rays {} [expr $w*10000] 16
    puts [timer 1 stop]

    acorn::prays rays
 }
}

