#!/bin/bash

for (( i=1; i<=64; i=i*4))
do 
	hadoop dfs -rmr /user/$USER/out*user/$USER/input
	time hadoop jar nl.uva.AssignmentMapreduce.jar A B out $i >& 
		times.out
done