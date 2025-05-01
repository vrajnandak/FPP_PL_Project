OCAMLC := ocamlfind ocamlopt
OCAMLFLAGS := -linkpkg
CPPC := g++
CPPFLAGS := -fopenmp

TARGETS := ocaml_MPI_WC ocaml_seq_WC cpp_parallel_WC

.PHONY: all clean benchmark

all: $(TARGETS)

ocaml_MPI_WC: MPI_WC.ml
	$(OCAMLC) -package unix,mpi,str $(OCAMLFLAGS) -o $@ $<

ocaml_seq_WC: WC_seq.ml
	$(OCAMLC) -package unix,str $(OCAMLFLAGS) -o $@ $<

cpp_parallel_WC: parallel_WC.cpp
	$(CPPC) $(CPPFLAGS) $< -o $@

benchmark: all
	@echo "\n=== Benchmarking Word Count Implementations ==="
	hyperfine --warmup 3 \
		'mpirun --bind-to core --map-by core -n 4 ./ocaml_MPI_WC' \
		'./ocaml_seq_WC' \
		'./cpp_parallel_WC' \
		--export-markdown wc_bench.md \
		--export-json wc_bench.json

clean:
	rm -f $(TARGETS) *.cm* *.o *.out *.json *.md

mpi: ocaml_MPI_WC
seq: ocaml_seq_WC
cpp: cpp_parallel_WC