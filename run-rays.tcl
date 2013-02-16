
proc run-rays { m } {
    set i 0
    foreach x { -12700 0 12700 } {
	foreach y { -12700 0 12700 } {
	    rays $i set px $x py $y pz 0 kx 0.0 ky 0.0 kz 1.0 vignetted 0
	    incr i
	}
    }

    GMT trace rays

    puts "x     	y     	z      	v"
    puts "-------	-------	-------	-"
    puts [join [map row [rays 0 end get px py pz vignetted] { join [map x $row { format "% 11.4f" $x }] \t }] \n]
}

