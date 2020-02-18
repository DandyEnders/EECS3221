echo PRIORITY TEST;
make -s ; make -s priority ; ./priority ${1:-./test/schedule.txt} ; make -s
