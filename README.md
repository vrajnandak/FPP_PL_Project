opam install domainslib


python3 run_and_benchmark.py


opam switch create 5.1.0
eval $(opam env)
opam install domainslib


ocamlfind ocamlopt -package domainslib -linkpkg -o parallel_sum parallel_sum.ml
./parallel_sum
g++ -fopenmp -O3 sum_parallel.cpp -o sum_parallel


sudo apt install hyperfine        # Ubuntu/Debian
brew install hyperfine 

# Move to project root
cd FPP_PL_PROJECT

# Rebuild OCaml and C++ just to be sure
dune build problems/mandelbrot_set/ocaml/mandelbrot.exe
g++ -O3 problems/mandelbrot_set/cpp/mandelbrot.cpp -o problems/mandelbrot_set/cpp/mandelbrot_cpp

# Benchmark using hyperfine
hyperfine './_build/default/problems/mandelbrot_set/ocaml/mandelbrot.exe' './problems/mandelbrot_set/cpp/mandelbrot_cpp'


dune clean dune build
dune build problems/matrixChain/ocaml/matrix_chain.exe
g++ -O3 problems/matrixChain/cpp/matrix_chain.cpp -o problems/matrixChain/cpp/matrix_chain_cpp
hyperfine --warmup 3 --min-runs 20 './_build/default/problems/matrixChain/ocaml/matrix_chain.exe' './problems/matrixChain/cpp/matrix_chain_cpp'
