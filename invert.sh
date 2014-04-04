#!/bin/bash
#Builds the invert module and runs it in default mode (binary)
g++-4.8 -std=c++11  invert.cpp lib/Posting.cpp lib/Index.cpp lib/Common.cpp -o invert
ls dumps/sorted | ./invert
