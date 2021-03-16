#!/bin/sh

cd tests
IN='*.smt2'

for i in $IN
    do
        ../main $i "`basename $i .smt2`.out"
        python ../check.py $i
    done
cd ..
