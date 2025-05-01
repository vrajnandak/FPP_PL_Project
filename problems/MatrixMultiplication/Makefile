compare_all_no_outputs: generate_matrices functional_mat_mul imp_mat_mul
	hyperfine './functional_mat_mul seq no' './functional_mat_mul partial no' './functional_mat_mul partial_parallel no' './functional_mat_mul partial_parallel_domainslib no' './imp_mat_mul seq no' './imp_mat_mul seq_opt no' './imp_mat_mul par no' './imp_mat_mul par_opt no'


compare_all_with_outputs: generate_matrices checker functional_mat_mul imp_mat_mul
	hyperfine './functional_mat_mul seq yes' './functional_mat_mul partial yes' './functional_mat_mul partial_parallel yes' './functional_mat_mul partial_parallel_domainslib yes' './imp_mat_mul seq yes' './imp_mat_mul seq_opt yes' './imp_mat_mul par yes' './imp_mat_mul par_opt yes'
	./checker

run_all_once_with_outputs: generate_matrices checker functional_mat_mul imp_mat_mul
	./functional_mat_mul seq yes
	./functional_mat_mul partial yes
	./functional_mat_mul partial_parallel yes
	./functional_mat_mul partial_parallel_domainslib yes
	./imp_mat_mul seq yes
	./imp_mat_mul par yes
	./imp_mat_mul seq_opt yes
	./imp_mat_mul par_opt yes
	./checker

generate_matrices: matrix_generator.cpp
	g++ -o generate_matrices matrix_generator.cpp
	./generate_matrices 640 1800 1800 640

checker: matrix_output_checker.cpp
	g++ -o checker matrix_output_checker.cpp

functional_mat_mul: mat_mul.ml
	ocamlfind ocamlopt -package domainslib -linkpkg -o functional_mat_mul mat_mul.ml

imp_mat_mul: mat_mul.cpp
	g++ -fopenmp -o imp_mat_mul mat_mul.cpp

clean:
	rm *.cmi *.cmx *.o matrix_a.txt matrix_b.txt functional_mat_mul imp_mat_mul checker *.json OCaml_mat_mul_seq.txt OCaml_mat_mul_partial_eval.txt OCaml_mat_mul_partial_eval_parallel.txt OCaml_mat_mul_partial_eval_parallel_domainslib.txt generate_matrices imp_par.txt imp_par_opt.txt imp_seq.txt imp_seq_opt.txt
