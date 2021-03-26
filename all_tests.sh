#!/bin/sh

cd tests
IN='*.smt2'

for i in lin/$IN
    do
        ../main $i lin/"`basename $i .smt2`.out"
        python ../check.py $i
    done

for i in exp/$IN
    do
        ../main $i exp/"`basename $i .smt2`.out"
        python ../check.py $i
    done
cd ..
