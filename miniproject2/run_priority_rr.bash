echo PRIORITY_RR TEST;
make -s ; make -s priority_rr ; ./priority_rr ${1:-./test/schedule.txt} ; make -s
