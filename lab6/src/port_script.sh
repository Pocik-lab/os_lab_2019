#!/bin/bash
port=20400
ip="127.0.0.1"

echo "Enter number of servers: "
read num

echo $num > /home/runner/oslab2019/lab6/src/Servers.txt
#-gt,(>) – больше
while [[ $num -gt 0 ]]
do
    ./server --port $port --tnum 4 &
    echo "$ip : $port" >> /home/runner/oslab2019/lab6/src/Servers.txt
    let num=$num-1
let port+=1
done
