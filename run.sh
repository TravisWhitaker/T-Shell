#!/bin/bash
# This file is used to run my shell with Valgrind
clear
echo "Running..."
valgrind --tool=memcheck --leak-check=full $1
