#!/bin/sh

cd tests
IN='*.in'

for i in $IN
    do
        ./../main $i "${i::-2}"'out'
    done
    
cd ../
