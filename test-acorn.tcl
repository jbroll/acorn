#!/Users/john/bin/tclkit8.6
#

lappend auto_path ./lib

package require arec
package require acorn

arec::typedef RayType {
    int		vignetted;
    double	px py pz
    double	kx ky kz
}

RayType new rays 1

rays 0 set px 1


puts [rays get]

puts [rays getptr]

rays
