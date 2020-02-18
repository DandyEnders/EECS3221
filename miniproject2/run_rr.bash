echo RR TEST;
make -s ; make -s rr ; ./rr ${1:-./test/schedule.txt} ; make -s
