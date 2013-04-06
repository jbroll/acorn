
proc timer { name action } {
    if { ![info exists ::timer($name,timer)] } { set ::timer($name,timer) 0 }

    switch $action {
	start { set ::timer($name,start) [clock milliseconds] }
	stop  { set ::timer($name,timer) [expr $::timer($name,timer) + [clock milliseconds] - $::timer($name,start)] }
    }

    format %.2f [expr $::timer($name,timer)/1000.0]
}

proc sleep { timer } {
    set sleepvar [namespace current]::sleep[clock seconds][expr int(rand() * 1000)]
    after $timer [list set $sleepvar 0]
    vwait $sleepvar
    unset $sleepvar
}

proc K { x y } { set x }
proc touch file {close [open $file a]}
proc cat { file } {
    if { [K [read [set fp [open $file rb]] 2] [close $fp]] eq "\xFF\xFE" } {
	return [string range [K [read -nonewline [K [set fp [open $file]] [fconfigure $fp -encoding unicode]]] [close $fp]] 1 end]
    } else {
	return [K [read -nonewline [K [set fp [open $file]] [fconfigure $fp -encoding utf-8]]] [close $fp]]
    }
}
proc echo { string { redirector - } { file - }} {
    switch -- $redirector {
	>       { set fp [open $file w]	}
	>>      { set fp [open $file a]	}
	default { set fp stdout		}
    }

    puts $fp $string
    if { [string compare $file -] } { close $fp }
}
