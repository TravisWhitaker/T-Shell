#!/bin/bash
# This file is used to build my shell
clear
echo "Building..."
gcc -g -std=gnu99 -Wall -ggdb -I ~/Code/C/Projects/include -lreadline tsh.c ../lib/* -o ./tsh
#sudo cp ./tsh /bin/
echo "Build Completed."
