#!/bin/bash


for ((j=1; j<=25;j++)); do
    n=1
    for ((i=1; i<=20;i++)) ; do
        echo "./cache_laten -b $n -s 64"
        ./cache_laten -b $n -s 64
        n=$((n * 2))
    done
done