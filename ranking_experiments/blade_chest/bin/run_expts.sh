#!/bin/bash

g++ real_bc_test.cpp

for i in {2,4,6,8};
do
	rfile="jester_result$i.txt"
	rm $rfile
	for j in {1..20};
	do
		outfile="out.txt"
		./BC -d $i -l 0.001 -S $j -M 2 ../datasets/jester/jester.txt $outfile
		./a.out $i < $outfile >> $rfile
	done
done
