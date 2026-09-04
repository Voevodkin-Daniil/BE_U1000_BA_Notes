#Connect settings
target extended-remote : 3336

#GDB Settings
set confirm off
set print pretty on
#set history save

#TUI Settings
set tui mouse-events off
#focus cmd
#tui disable

#BMCU Settings
alias bmcu=monitor
set remote hardware-breakpoint-limit 3

set mem inaccessible-by-default off 
mem 0x80000000 0x81000000 ro nocache
mem 0x90000000 0x91000000 ro nocache
mem 0xa0000000 0xa0040000 ro nocache
enable mem 1
enable mem 2
disable mem 3
