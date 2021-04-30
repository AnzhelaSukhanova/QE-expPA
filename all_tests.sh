#!/bin/bash

cd tests
IN='*.smt2'

if [ -z $1 ] || [ $1 == "simp_lin" ]
then
	for in in simp_lin/$IN
    	do
    	    out=simp_lin/"`basename $in .smt2`.out"
    	    ../main $in $out
    	    python ../check.py $in $out
    	done
fi

if [ -z $1 ] || [ $1 == "simp_exp" ]
then
	for in in simp_exp/$IN
    	do
    	    out=simp_exp/"`basename $in .smt2`.out"
    	    ../main $in $out
    	    python ../check.py $in $out
    	done
fi

if [ -z $1 ] || [ $1 == "simp_mix" ]
then
	for in in simp_mix/$IN
	    do
	        out=simp_mix/"`basename $in .smt2`.out"
	        ../main $in $out
	        python ../check.py $in $out
	    done
fi

if [ -z $1 ] || [ $1 == "lin" ]
then    
	for in in lin/$IN
	    do
	        out=lin/"`basename $in .smt2`.out"
	        ../main $in $out
	        python ../check.py $in $out
	    done
fi

if [ -z $1 ] || [ $1 == "exp" ]
then    
	for in in exp/$IN
	    do
	        out=exp/"`basename $in .smt2`.out"
	        ../main $in $out
	        python ../check.py $in $out
	    done
fi

rm check
cd ..

