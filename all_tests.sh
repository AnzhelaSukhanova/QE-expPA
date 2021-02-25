#!/bin/sh

cd tests
IN='*.in'

for i in $IN
    do
        ../main $i "`basename $i .in`.out"
        python ../check.py $i
    done
cd ..
