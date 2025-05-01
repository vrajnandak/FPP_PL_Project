# MandelBrot 
## Build Ocaml 
dune build problems/mandelbrot_set/ocaml/mandelbrot.exe
## Make mandelbrot set
dune exec problems/mandelbrot_set/ocaml/mandelbrot.exe
## Build C++ 
g++ -O3 problems/mandelbrot_set/cpp/mandelbrot.cpp -o problems/mandelbrot_set/cpp/mandelbrot_cpp
## Make mandelbrot set
./problems/mandelbrot_set/cpp/mandelbrot_cpp
## Benchmark
hyperfine './_build/default/problems/mandelbrot_set/ocaml/mandelbrot.exe' './problems/mandelbrot_set/cpp/mandelbrot_cpp'


# Matrix Chain
## Build Ocaml 
dune build problems/matrixChain/ocaml/matrix_chain.exe
## Build C++
g++ -O3 problems/matrixChain/cpp/matrix_chain.cpp -o problems/matrixChain/cpp/matrix_chain_cpp
## Benchmark
hyperfine --warmup 3 --min-runs 20 './_build/default/problems/matrixChain/ocaml/matrix_chain.exe' './problems/matrixChain/cpp/matrix_chain_cpp'