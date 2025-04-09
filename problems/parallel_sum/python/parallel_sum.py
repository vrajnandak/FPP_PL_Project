import multiprocessing
import time

def chunk_sum(chunk):
    return sum(chunk)

def parallel_sum(arr, num_processes):
    chunk_size = len(arr) // num_processes
    chunks = [arr[i:i + chunk_size] for i in range(0, len(arr), chunk_size)]
    with multiprocessing.Pool(num_processes) as pool:
        results = pool.map(chunk_sum, chunks)
    return sum(results)

if __name__ == "__main__":
    n = 10_000_000
    arr = list(range(1, n + 1))

    start = time.time()
    result = parallel_sum(arr, multiprocessing.cpu_count())
    end = time.time()

    print("Sum =", result)
    print(f"Time taken (Python) = {end - start:.4f} seconds")
