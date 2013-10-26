#!/bin/bash
# This file is used to build my shell
clear
echo "Building..."
gcc -g -std=gnu99 -ggdb -lreadline tsh.c ../lib/* -o ./tsh
#sudo cp ./tsh /bin/
echo "Build Completed."
