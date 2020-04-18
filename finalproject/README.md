# Summary
run_program.bash is a script to clean, make, run, clean and output input.txt's 
scheduling process result to output.txt.

# Manual run
If you want to manually run this program, do:
```
make schedule
```
to compile the program, and
```
./schedule ${1:-./input.txt} ${2:-33} ${3:-./output.txt}
```
where $1 corresponds to input file with default ./input.txt, and 
$2 corresponds to initial head position, and
$3 corresponds to output file with default ./output.txt.

# Scripted run
To run convenently the above, do the following:
```
#default ./input.txt input and ./output.txt
bash run_program.bash

#custom input and output
bash run_program.bash <input_txt> <initial_head_position> <output_txt>
```
Then, look at output.txt for the process result.