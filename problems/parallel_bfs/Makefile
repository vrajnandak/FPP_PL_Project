target_cpp: bfs_seq bfs_parallel
        hyperfine './bfs_seq' './bfs_parallel'

bfs_seq: bfs_seq.cpp large_graph_maker.cpp
        g++ -o bfs_seq bfs_seq.cpp large_graph_maker.cpp

bfs_parallel: bfs_parallel.cpp large_graph_maker.cpp
        g++ -fopenmp -o bfs_parallel bfs_parallel.cpp large_graph_maker.cpp
