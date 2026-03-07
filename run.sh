#!/bin/bash
cd build
cmake --build .
./kilo ../test_input.txt
cd ..
