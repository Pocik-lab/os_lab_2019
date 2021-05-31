#!/bin/bash
port=20001
ip="127.0.0.1"

echo "Enter number of servers: "
read num

echo $num > /home/runner/oslab2019/lab6/src/Servers.txt
#-gt,(>) – больше
while [[ $num -gt 0 ]]
do
#Чтобы получить список всех прослушивающих TCP-портов с типом lsof
if !(lsof -Pi :$port -sTCP:LISTEN -t >/dev/null);
 then
    ./server --port $port --tnum 3 &
    echo "$ip : $port" >> /projects/os_lab_2019/lab6/src/Servers.txt
    let num=$num-1
fi
let port+=1
done
