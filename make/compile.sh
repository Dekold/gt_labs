#!/bin/bash
mkdir build 2>/dev/null
g++ src/matrix.h src/matrix.cpp src/L1.cpp -o build/L1
