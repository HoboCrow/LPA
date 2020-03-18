#!/bin/bash

make debug
rm ./prog_out/*
rm ./visout/*
./a.out < "./inputs/$1.txt"
i=1
for file in ./prog_out/*; do
    python visualizer.py $i "./inputs/$1.txt" $file ./visout
    i=$(($i+1))
done