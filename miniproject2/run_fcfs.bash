echo FCFS TEST;
make -s ; make -s fcfs ; ./fcfs ${1:-./test/schedule.txt} ; make -s
