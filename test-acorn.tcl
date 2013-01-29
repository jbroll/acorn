#!/usr/bin/env tclkit8.6
#

lappend auto_path ./lib

package require arec
package require acorn

source tcloo.tcl


acorn::init

acorn::Rays new  rays 1

set fields { aper R K n thinkness }




acorn::surface       create  Start R 0 K 0 thickness 10000

acorn::surface-group create  P type non-sequential  {
    P1 { }
    P2 { }
    P3 { }
    P4 { }
    P5 { }
    P6 { }
    P7 { }
}

acorn::surface-group create  S type non-sequential {
    S1 { R 401 }
    S2 { R 402 }
    S3 { R 403 }
    S4 { R 404 }
    S5 { R 405 }
    S6 { R 406 }
    S7 { R 407 }
}

acorn::surface       create  Focus R 400

acorn::trace rays Start P S Focus

