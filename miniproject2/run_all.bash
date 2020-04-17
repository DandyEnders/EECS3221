TEST_DATA=${1:-./test/schedule.txt}

echo "Running \"$TEST_DATA\" as test data"
bash run_fcfs.bash $TEST_DATA;
bash run_sjf.bash $TEST_DATA;
bash run_priority.bash $TEST_DATA;
bash run_rr.bash $TEST_DATA;
bash run_priority_rr.bash $TEST_DATA;