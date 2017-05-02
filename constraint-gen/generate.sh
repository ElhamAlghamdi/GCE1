#!/bin/bash
NETWORK=$1
PERCENTAGE=$2
NUM=$3
NUM1=$4
RUNS=100

echo "+++ Generate constraint files for $NETWORK with $PERCENTAGE% ($NUM) constraints"
for R in $(seq -f "%02g" 1 $RUNS); do
	python generate.py $NETWORK --seed $PERCENTAGE -m $NUM -c $NUM1 -o constraints-Face$PERCENTAGE-$R.txt
done




