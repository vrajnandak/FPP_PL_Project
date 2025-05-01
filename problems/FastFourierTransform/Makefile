# Compiler configurations
OCAMLC := ocamlfind ocamlopt
OCAMLFLAGS := -linkpkg
CPPC := g++
CPPFLAGS := -fopenmp
CPPFLAGS_SEQ :=

TARGETS := ocaml_FFT ocaml_FFT_seq cpp_FFT cpp_FFT_seq

.PHONY: all clean benchmark

all: $(TARGETS)

ocaml_FFT: FFT.ml
	$(OCAMLC) -package domainslib,unix $(OCAMLFLAGS) -o $@ $<

ocaml_FFT_seq: FFT_seq.ml
	$(OCAMLC) $(OCAMLFLAGS) -o $@ $<

cpp_FFT: FFT.cpp
	$(CPPC) $(CPPFLAGS) $< -o $@

# C++ Sequential FFT
cpp_FFT_seq: FFT_seq.cpp
	$(CPPC) $(CPPFLAGS_SEQ) $< -o $@

# Benchmarking
benchmark: all
	@echo "\n=== Benchmarking FFT Implementations ==="
	hyperfine --warmup 3 \
		'./ocaml_FFT' \
		'./ocaml_FFT_seq' \
		'./cpp_FFT' \
		'./cpp_FFT_seq' \
		--export-markdown fft_bench.md \
		--export-json fft_bench.json
		
clean:
	rm -f $(TARGETS) *.cm* *.o *.out *.json *.md

# Helper targets
ocaml_parallel: ocaml_FFT
ocaml_seq: ocaml_FFT_seq
cpp_parallel: cpp_FFT
cpp_seq: cpp_FFT_seq