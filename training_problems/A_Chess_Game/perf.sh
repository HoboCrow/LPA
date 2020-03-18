#!/bin/bash

time $(
    for i in {1..1000};
    do $(./a.out < ./test.txt > /dev/null);
    done
)