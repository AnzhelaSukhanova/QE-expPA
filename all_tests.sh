#!/bin/bash

cd tests
IN='*.smt2'

if [ -z $2 ]
then
	CASE="simp_lin simp_exp simp_mix lin exp benchmarks"
else
	CASE=$2
fi
if [ ! -z $1 ] && [ $1 != "-check" ]
then
	CASE=$1
fi


for case in $CASE
do
	for in in $case/$IN
    do
    	mkdir -p $case/out
        out=$case/out/"`basename $in .smt2`.out"
   	   	../main $in $out
   	   	if [ ! -z $1 ] && [ $1 == "-check" ]
   	   	then
   	   		python ../check.py $in $out
   	 	fi
   	 done
done

if [ ! -z $1 ] && [ $1 == "-check" ]
then
	rm check
fi

cd ..

