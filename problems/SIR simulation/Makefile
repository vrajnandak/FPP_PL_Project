OCAMLC := ocamlfind ocamlopt
OCAMLFLAGS := -linkpkg
CPPC := g++
CPPFLAGS := -fopenmp

TARGETS :=  parmap_SIR_simple parmap_SIR SIR_parallel_cpp SIR_parallel_simple_cpp

.PHONY: all clean benchmark

all: $(TARGETS)

parmap_SIR_simple: parmap_SIR_simple.ml
	$(OCAMLC) -package parmap $(OCAMLFLAGS) -o $@ $<

parmap_SIR: parmap_SIR.ml
	$(OCAMLC) -package parmap $(OCAMLFLAGS) -o $@ $<

SIR_parallel_cpp: SIR_parallel.cpp
	$(CPPC) $(CPPFLAGS) $< -o $@

SIR_parallel_simple_cpp: SIR_parallel_simple.cpp
	$(CPPC) $(CPPFLAGS) $< -o $@

benchmark: all
	@echo "\n=== Benchmarking Simple Implementations ==="
	hyperfine --warmup 3 \
		'./parmap_SIR_simple' \
		'./SIR_parallel_simple_cpp' \
		--export-markdown simple_bench.md
	
	@echo "\n=== Benchmarking Optimized Implementations ==="
	hyperfine --warmup 3 \
		'./parmap_SIR' \
		'./SIR_parallel_cpp' \
		--export-markdown optimized_bench.md

clean:
	rm -f $(TARGETS) *.cm* *.o *.out

parmap_simple: parmap_SIR_simple
parmap: parmap_SIR
cpp_simple: SIR_parallel_simple_cpp
cpp: SIR_parallel_cpp