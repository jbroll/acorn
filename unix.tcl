
proc sleep { timer } {
    set sleepvar [namespace current]::sleep[clock seconds][expr int(rand() * 1000)]
    after $timer [list set $sleepvar 0]
    vwait $sleepvar
    unset $sleepvar
}

proc K { x y } { set x }
proc touch file {close [open $file a]}
proc cat { file { encoding utf-8 } } { K [read -nonewline [K [set fp [open $file]] [fconfigure $fp -encoding $encoding]]] [close $fp] }
proc echo { string { redirector - } { file - }} {
    switch -- $redirector {
	>       { set fp [open $file w]	}
	>>      { set fp [open $file a]	}
	default { set fp stdout		}
    }

    puts $fp $string
    if { [string compare $file -] } { close $fp }
}
