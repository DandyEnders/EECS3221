#!/bin/bash
echo "Cleaning"
make -s
echo "Compiling schedule"
make -s schedule
echo Running the schedule program on input \"${1:-./input.txt}\" 
echo and with initial head position of \"${2:-33}\" output \"${3:-./output.txt}\"
./schedule ${1:-./input.txt} ${2:-33} ${3:-./output.txt}
echo "Cleaning"
make -s
# for debug, enable next
# diff -yZ ${2:-./output.txt} correct_output.txt ; echo ""