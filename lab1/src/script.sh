#!/bin/bash

string1=$(pwd)
string2=$(date)

string=$string1$string2

echo $string

PATH="My path for task 3"
echo $PATH
unset PATH
