# Harnessing Functional Programming For Parallelism

The project focuses on leveraging functional programming concepts such as partial evaluation, actors, futures/promises, immutable data structures etc for parallel computing. To demonstrate their effectiveness, a range of real-world problems were identified and implemented using various functional programming features in OCaml. These implementations were then compared with equivalent solutions in C++. Several libraries and modules were employed throughout the project, including ParMap, Domainslib, OpenMP, MPI, and Lwt.

Note: All implementations require OCaml v5.0.0


# Parallel Word Count Implementations
## Implementations
- MPI_WC.ml: OCaml MPI version
- WC_seq.ml: OCaml sequential version
- parallel_WC.cpp: C++ OpenMP version

## Compilation
```
bash

# OCaml MPI
make mpi # to run executable independently: mpirun --bind-to core --map-by core -n 4 ./ocaml_MPI_WC

# OCaml Sequential
make seq

# C++ Parallel
make cpp

# Build all
make all

# Build all, and then benchmark using hyperfine
make benchmark
```


# Parallel SIR Model Simulations

## Implementations
- parmap_SIR.ml: OCaml Parmap
- parmap_SIR_simple.ml: OCaml Parmap - simple implementation without movement
- SIR_parallel.cpp: C++ OpenMP
- SIR_parallel_simple.cpp: C++ OpenMP - simple implementation without movement

## Build
```
bash

# OCaml with Parmap
make parmap
make parmap_simple

# C++ Parallel
make cpp
make cpp_simple

# Full build
make all

# Benchmark
make benchmark
```


# Parallel FFT Implementations

## Implementations
- FFT.ml: OCaml Domainslib (parallel)
- FFT_seq.ml: OCaml sequential
- FFT.cpp: C++ OpenMP
- FFT_seq.cpp: C++ sequential

## Compilation
```
bash

# OCaml Parallel
make ocaml_parallel

# OCaml Sequential
make ocaml_seq

# C++ Parallel
make cpp_parallel

# Build all
make all

# Benchmark
make benchmark
```

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


# Matrix Multiplication, PageRank
Run the following command after changing to the appropriate directory to see the output.
bash
'''
  make
'''

