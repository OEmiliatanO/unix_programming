#!/bin/bash
#1
param2_list=(1000 800 500 300)
param3_list=(10 100 1000)

for param2 in "${param2_list[@]}"; do
    for param3 in "${param3_list[@]}"; do
        output_file="res_1000_${param2}_${param3}_3.txt"
        ./main 1000 $param2 $param3 3 &> "$output_file"
    done
done

#2
for x in {1..10}; do
    for param3 in "${param3_list[@]}"; do
        output_file="res_1000_500_${param3}_${x}.txt"
        ./main 1000 500 $param3 $x &> "$output_file"
    done
done
