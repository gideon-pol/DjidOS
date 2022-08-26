target remote localhost:1234
symbol-file dist/x86_64/kernel.bin

source -v gdb-debug.py

define nub
  python nextUntilBreakpoint()
end

b Handler0xE
b Scheduler::ScheduleNext
b src/kernel/memory/pmm.cpp:30

set pagination off
set logging file gdb.log
set logging on
set $i = 0

commands 2
  set $i=1000000
  print "commands 2 : %d",$i
end

continue

define nub2
    while ( $i < 1000000 )
    step
    # next
    # continue
    set $i = $i + 1
    end
end
