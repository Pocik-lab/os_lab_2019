#!/bin/bash 
 
for ((i = 1; i < 151; i++))
do
od -A n -t d -N 1 /dev/random >> $1
done

cat $1
