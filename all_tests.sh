#!/bin/sh

cd tests
IN='*.in'

for i in $IN
    do
        ./../main $i 'out'"${i::-3}"
    done
    
cd ../
