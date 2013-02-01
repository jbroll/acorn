#!/bin/sh
#
# Brian made this with my help.
#

column -a absm 			| 
compute 'absm=abs(m)'		| 
sorttable -n n absm m		| 
column -N i 			| 
compute '
.mparity = (m? m / abs(m):0)
.nparity = (i%2 * 2 - 1)
if ((m<0) && nparity>0) { .parity = 1 } else {parity = -1}
'  				|
sorttable -n n absm parity	|
column -n n m z dx dy
