#!/bin/bash

echo "graph_dim,program,runtime" >> runtimes.csv

#matrix_sizes=(
#	"1000 5000"
#	"1000 10000"
#	"1000 50000"
#	"1000 100000"
#	"1000 300000"
#	"1000 400000"
#)

matrix_sizes=(
	"10 22"
	"50 600"
	"100 2500"
	"500 60000"
	"1000 150000"
	"3000 2200000"
)


programs=(
	"./imp_seq"
	"./imp_par"
	"./func_seq"
	"./func_par"
)

for size in "${matrix_sizes[@]}"
do
	echo "Generating graph for dimensions: $size"
	./create_graph $size

	size_str=$(echo $size | tr ' ' '_')

	for cmd in "${programs[@]}"
	do
		echo "Running: $cmd for matrix size: $size"
		cmd_name=$(echo $cmd | tr ' ' '_' | tr -d './')
		json_file="benchmark_${cmd_name}_${size_str}.json"

		hyperfine --export-json "$json_file" "$cmd" > /dev/null

		runtime=$(jq '.results[0].mean' "$json_file")

		printf '"%s","%s",%s\n' \
			"$size" \
			"$cmd" \
			"$runtime" \
			>> runtimes.csv
	done
done

echo "Benchmarking complete. Results saved to runtimes.csv. Individual runtimes saved to json files"
