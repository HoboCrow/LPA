#!/bin/bash

show_errors=''
compile_debug=''

while getopts 'ed' flag; do
    case "$flag" in
        e) show_errors='true' ;;
        d) compile_debug='true' ;;
    esac
done

if [ "$compile_debug" = 'true' ]
then
    echo "Recompiling in debug..."
    make debug
else
    echo "Recompiling ..."
    make
fi
error=$?
if [ $error -ne 0 ]
then
    exit
fi
echo "Done."
outputfile="temp.txt" #TODO change to pipe

printf "\t Tests:\n"

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
        if [ "$show_errors" = 'true' ]
        then
            cat $outputfile
            echo ""
        fi
    else
        echo 🛑 $(basename $filename) "Something went wrong: " && cat $outputfile
    fi
done

rm $outputfile