#!/bin/bash 
 
make clean && make cleankeys && make

#Generate keys based on amount of bits (seed = 314 for consistant testing)
count=$((50))
while [ $count -le 4096 ]
do	
	echo "Bits: "$count
	./keygen -s 314 -b $count
	((count++))
done
#If program never crashes then ./keygen successfully generates keys based on bits


#Generate keys based on seed (bits = 1024 for consistant testing)
count2=$((1))
while [ $count2 -le 1000 ]
do 
	echo "Seed: "$count2
	./keygen -s $count2
	((count2++))
done
#If program never crashes then ./kegen successfully generates kets based on seed
