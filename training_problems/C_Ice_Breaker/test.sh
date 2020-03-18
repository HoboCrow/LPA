#!/bin/bash
echo -n "Recompiling..."
g++ -Wall ice.cpp
error=$?
if [ $error -ne 0 ]
then
    exit
fi
echo " done."
outputfile="temp.txt" #TODO change to pipe

for filename in ./inputs/*; do
    #echo $(basename $filename)
    diff -y --suppress-common-lines <( ./a.out < $filename ) ./outputs/$(basename $filename) &> $outputfile
    error=$?
    if [ $error -eq 0 ]
    then
        echo ✅ $(basename $filename)
    elif [ $error -eq 1 ]
    then
        echo ❌ $(basename $filename)
        if [ "$1" = "-e" ]
        then
            cat $outputfile
        fi
    else
        echo 🛑 $(basename $filename) "Something went wrong: " && cat $outputfile
    fi
done

rm $outputfile