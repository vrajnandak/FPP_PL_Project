#!/bin/bash

echo "matrix_size,program,runtime" >> runtimes.csv

matrix_sizes=(
	"100 300 300 100"
	"200 600 600 200"
	"300 900 900 300"
	"400 1200 1200 400"
	"500 1500 1500 500"
	"640 1800 1800 640"
	"800 2000 2000 800"
	"1000 2500 2500 1000"
	"1200 3000 3000 1200"
	"1500 3500 3500 1500"
)

programs=(
	"./functional_mat_mul seq no"
	"./functional_mat_mul partial no"
	"./functional_mat_mul partial_parallel no"
	"./functional_mat_mul partial_parallel_domainslib no"
	"./imp_mat_mul seq no"
	"./imp_mat_mul seq_opt no"
	"./imp_mat_mul par no"
)

for size in "${matrix_sizes[@]}"
do
	echo "Generating matrices for size: $size"
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
