#!/bin/bash
make -s
make -s schedule
./schedule ${1:-./input.txt} ${2:-./output.txt}
make -s
