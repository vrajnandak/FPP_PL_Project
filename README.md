opam install domainslib


python3 run_and_benchmark.py


opam switch create 5.1.0
eval $(opam env)
opam install domainslib


ocamlfind ocamlopt -package domainslib -linkpkg -o parallel_sum parallel_sum.ml
./parallel_sum
g++ -fopenmp -O3 sum_parallel.cpp -o sum_parallel