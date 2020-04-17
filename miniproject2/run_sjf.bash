echo SJF TEST;
make -s ; make -s sjf ; ./sjf ${1:-./test/schedule.txt} ; make -s
