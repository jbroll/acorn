#!/usr/bin/env tclkit8.6
#
#parray tcl_platform

package require tcltest

lappend auto_path lib ../lib

package require acorn


::tcltest::configure -testdir [file dirname [file normalize [info script]]]/test -singleproc 1

::tcltest::configure {*}$argv
::tcltest::runAllTests

