#!/usr/bin/env tclkit8.6
#
lappend auto_path ./lib arec/lib

package require tcltest
package require acorn

source util/rays.tcl

acorn::ZMX create B source zmx-models/p8_110912af_072512ac.zmx

#puts [B config 1]
#puts [B 70 get thickness]

set w1 [B get wavelength 1 wave]
set w2 [B get wavelength 2 wave]
set w3 [B get wavelength 3 wave]

set w $w1

acorn::mkrays rays nx 3000 x0 -128 x1 128 ny 3000 y0 -128 y1 128 circle 1

#acorn::Rays create rays  1
#rays set py 128 kz 1
#acorn::Rays create trak 70
#puts [rays get]
#puts [rays length]

puts Trace

timer 1 start
B trace rays {} [expr $w*10000] 16
puts [timer 1 stop]

#puts [acorn::rays-rms rays 1]

#B print
#acorn::prays rays

