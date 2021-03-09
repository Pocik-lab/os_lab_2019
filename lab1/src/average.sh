#!/bin/bash

var1=0;
string1=$(cat $1 | wc -l)

for number in $(cat $1) 
do
let "var1=var1+number"
done

let "var1=var1/string1"

echo "numbers : $string1"
echo "res = $var1"