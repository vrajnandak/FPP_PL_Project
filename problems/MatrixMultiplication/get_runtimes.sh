#!/bin/bash

echo "matrix_size,program,runtime" >> runtimes.csv

matrix_sizes=(
	"640 100 100 640"
	"640 200 200 640"
	"640 500 500 640"
	"640 1000 1000 640"
	"640 1500 1500 640"
	"640 2000 2000 640"
)


programs=(
	"./functional_mat_mul seq no"
	"./functional_mat_mul partial no"
	"./functional_mat_mul partial_parallel no"
	"./functional_mat_mul partial_parallel_domainslib no"
	"./imp_mat_mul seq no"
	"./imp_mat_mul seq_opt no"
	"./imp_mat_mul par no"
	"./imp_mat_mul par_opt no"
)

for size in "${matrix_sizes[@]}"
do
	echo "Generating matrices for size: $size"
	./generate_matrices $size

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
