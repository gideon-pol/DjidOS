# gdb will 'recognize' this as python
#  upon 'source pygdb-logg.py'
# however, from gdb functions still have
#  to be called like:
#  (gdb) python print logExecCapture("bt")

import sys
import gdb
import os

def logExecCapture(instr):
  # /dev/shm - save file in RAM
    ltxname="/dev/shm/c.log"

    gdb.execute("set logging file "+ltxname) # lpfname
    print("hullo1")

    gdb.execute("set logging redirect on")
    print("hullo2")

    gdb.execute("set logging overwrite on")
    print("hullo3")

    gdb.execute("set logging on")
    print("hullo4")

    gdb.execute(instr)
    gdb.execute("set logging off")

    print("hullo")


    replyContents = open(ltxname, 'r').read() # read entire file
    return replyContents

# next until breakpoint
def nextUntilBreakpoint():
    isInBreakpoint = -1
    # as long as we don't find "Breakpoint" in report:
    while isInBreakpoint == -1:
        try:
            REP=logExecCapture("s")
        except:
            print("whoops")

        isInBreakpoint = REP.find("Breakpoint")
        print("Loop: " + REP)