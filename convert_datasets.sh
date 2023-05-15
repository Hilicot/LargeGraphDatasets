#!/bin/bash

out_folder="processed/ascii"

for dataset in raw/as* ; do
    let counter=counter+1
    #if [ $counter -le 5 ]; then
        #echo "Skipping# $pair"
    #    continue
    #fi
    echo "Processing $dataset"
    ./prog $dataset $out_folder $counter
done 
#python3 /home/porro/telecho/telecho.py marco done