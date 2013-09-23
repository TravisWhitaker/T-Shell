#!/bin/bash
# This file is used to build my shell
clear
echo "Building..."
gcc -g --std=gnu99 -ggdb T-Shell.c ../lib/* -o tsh
echo "Build Completed."
