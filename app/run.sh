#/bin/bash

path="$(pwd)/data/data.bin"
./build/opz $path >> $(pwd)/output/output.txt
