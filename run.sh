#!/bin/bash
#This runs everything.
find nz2 -type f | python3 parse.py
./sorter.sh
ls dumps/sorted | ./invert
