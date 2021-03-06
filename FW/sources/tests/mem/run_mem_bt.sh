#!/bin/bash
#-------------------------------------------------------
# this script call mem_bt multiple times
# it also count number call which cause error
#-------------------------------------------------------
index=0
err=0
app="./build/mem_bt"


# check if test application is built
if [ ! -f "$app" ]; then 
    echo "$app dose not exist"
    echo "run make first"
    exit 1
fi

# check for number of iteration 
if [ -z "$1" ]; then
    echo "Usage: "$0" <number of iteration>"
    exit 2
fi

# call test application in loop
while [ $index -lt $(($1)) ] 
do
    ./$app
    #check return value 
    if [ $? -ne 0 ]; then
        echo "error in iteration $index"
        err=$(( $err + 1))
    fi
    index=$(( $index + 1))
done

echo "Total iteration: $index"
echo "Total errors: $err"

if [ "$err" -ne 0 ]; then
    exit 3
else
    exit 0
fi

