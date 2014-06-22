#! /bin/bash

#define the prefix length:2-10

for line in `cat english-dict-lower`
do
        for((j=2;j<13;j++))
        do
        if(( ${#line}>=$j));then
                echo "$(($j))-${line:0:$j}" >> test.txt
        fi 
        #add this condition to create enough prefix
        if((${#line}>$j));then
                echo "$(($j))-${line:1:$j}" >>test.txt
        fi
        done
done

sort -n test.txt | sed 's/.*-//' | uniq > prefix.txt
