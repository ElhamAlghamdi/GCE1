#!/bin/bash
#./run.sh Caltech 1
NETWORK=$1
PERCENTAGE=$2
RUNS=100

echo "+++ Apply GCE for $NETWORK with $PERCENTAGE% ($NUM) constraints"
for R in $(seq -f "%02g" 1 $RUNS); do
	./GCECommunityFinder $NETWORK.dat constraints-Face$PERCENTAGE-$R.txt > results-Face$PERCENTAGE-$R.dat
done



