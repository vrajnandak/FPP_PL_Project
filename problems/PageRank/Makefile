all: create_graph functional_pagerank imp_pagerank
	hyperfine './func_seq' './func_par' './imp_seq' './imp_par'

get_runtimes: create_graph_exec functional_pagerank imp_pagerank
	./run_bash.sh

create_graph_exec: create_graph.ml
	ocamlc -o create_graph create_graph.ml

create_graph: create_graph_exec
	./create_graph 500 98000

functional_pagerank: func_seq func_par

func_seq: pagerank_seq.ml
	ocamlfind ocamlopt -linkpkg -package lwt_ppx,lwt.unix pagerank_seq.ml -o func_seq

func_par: pagerank_par.ml
	ocamlfind ocamlopt -linkpkg -package lwt_ppx,lwt.unix pagerank_par.ml -o func_par

imp_pagerank: imp_seq imp_par

imp_seq: pagerank_seq.cpp
	g++ pagerank_seq.cpp -o imp_seq

imp_par: pagerank_par.cpp
	g++ -fopenmp pagerank_par.cpp -o imp_par

clean:
	rm *.cmi *.cmx *.o func_seq func_par imp_seq imp_par create_graph graph.txt *.json runtimes.csv *.cmo
