#!/bin/sh

cd tests
IN='*.smt2'

for in in lin/$IN
    do
        out=lin/"`basename $in .smt2`.out"
        ../main $in $out
        python ../check.py $in $out
    done

for in in exp/$IN
    do
        out=exp/"`basename $in .smt2`.out"
        ../main $in $out
        python ../check.py $in $out
    done

for in in mix/$IN
    do
        out=mix/"`basename $in .smt2`.out"
        ../main $in $out
        python ../check.py $in $out
    done
rm check
cd ..

