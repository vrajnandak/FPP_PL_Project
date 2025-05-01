# Harnessing Functional Programming For Parallelism

The project focuses on leveraging functional programming concepts such as partial evaluation, actors, futures/promises, immutable data structures etc for parallel computing. To demonstrate their effectiveness, a range of real-world problems were identified and implemented using various functional programming features in OCaml. These implementations were then compared with equivalent solutions in C++. Several libraries and modules were employed throughout the project, including ParMap, Domainslib, OpenMP, MPI, and Lwt.

Note: All implementations require OCaml v5.0.0

# Matrix Multiplication, PageRank
Run the following command after changing to the appropriate directory to see the output.
bash
'''
  make
'''

# Parallel Word Count Implementations
## Implementations
- MPI_WC.ml: OCaml MPI version
- WC_seq.ml: OCaml sequential version
- parallel_WC.cpp: C++ OpenMP version

## Compilation
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


# Parallel SIR Model Simulations

## Implementations
- parmap_SIR.ml: OCaml Parmap
- parmap_SIR_simple.ml: OCaml Parmap - simple implementation without movement
- SIR_parallel.cpp: C++ OpenMP
- SIR_parallel_simple.cpp: C++ OpenMP - simple implementation without movement

## Build
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



# Parallel FFT Implementations

## Implementations
- FFT.ml: OCaml Domainslib (parallel)
- FFT_seq.ml: OCaml sequential
- FFT.cpp: C++ OpenMP
- FFT_seq.cpp: C++ sequential

## Compilation
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
