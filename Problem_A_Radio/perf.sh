#!/bin/bash

time $(
    for i in {1..1000};
    do $(./a.out < ./inputs/base.txt > /dev/null);
    done
)