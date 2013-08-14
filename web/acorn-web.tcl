#!/usr/bin/env tclkit8.6

set wobble ../../wobble

set env(ACORN_SURFACES) /Users/john/src/acorn/surfaces
set env(ACORN_GLASS)	/Users/john/src/acorn/glass

lappend auto_path /Users/john/src/acorn/lib
package require acorn

source $wobble/wibble/icc.tcl
source $wobble/wibble/wibble.tcl
source $wobble/wibble/utility.tcl

source $wobble/wibble/zones/404.tcl

source $wobble/wibble/zones/staticfile.tcl
source $wobble/wibble/zones/template.tcl

source $wobble/wibble/zones/authenticate.tcl
source $wobble/wibble/zones/authorize.tcl

source $wobble/wibble/modules/session-file.tcl

proc wibble::entropy {}     { set fp [open /dev/random];  K [read $fp 16] [close $fp] }


    wibble::handle /files	staticfile	root files

    wibble::handle /		template.tml 	root .		sources Template.lib
    wibble::handle /		 	 404

    wibble::listen 8080
    vwait forever
