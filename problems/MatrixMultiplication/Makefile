run_all_no_outputs: generate_matrices functional_mat_mul imp_mat_mul
	hyperfine './functional_mat_mul seq no' './functional_mat_mul partial no' './functional_mat_mul partial_parallel no' './functional_mat_mul partial_parallel_domainslib no' './imp_mat_mul seq no' './imp_mat_mul seq_opt no' './imp_mat_mul par no'


run_all_with_outputs: generate_matrices checker functional_mat_mul imp_mat_mul
	hyperfine './functional_mat_mul seq yes' './functional_mat_mul partial yes' './functional_mat_mul partial_parallel yes' './functional_mat_mul partial_parallel_domainslib yes' './imp_mat_mul seq yes' './imp_mat_mul seq_opt yes' './imp_mat_mul par yes'
	./checker

generate_matrices: matrix_generator.cpp
	g++ -o generate_matrices matrix_generator.cpp
	./generate_matrices 640 1800 1800 640

checker: matrix_output_checker.cpp
	g++ -o checker matrix_output_checker.cpp

functional_mat_mul: mat_mul.ml
	ocamlfind ocamlopt -package domainslib -linkpkg -o functional_mat_mul mat_mul.ml

imp_mat_mul: mat_mul.cpp
	g++ -o imp_mat_mul mat_mul.cpp

clean:
	rm *.txt imp_mat_mul functional_mat_mul *.cmi *.cmo *.cmx *.o generate_matrices
