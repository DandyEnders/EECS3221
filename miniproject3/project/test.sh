#!/bin/bash -e
echo "Compiling"
make -s
make -s mmu
echo "Running"
./mmu ./data/BACKING_STORE.bin ./data/addresses.txt ./output/output.csv
echo "Done. Please look at README.md for where outputs are located."
echo "Cleaning"
make -s