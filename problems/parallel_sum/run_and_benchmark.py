import subprocess
import time
import json
import os

RESULTS_PATH = "results/benchmark.json"

def run_python():
    print("Running Python version...")
    start = time.time()
    subprocess.run(["python3", "python/sum_parallel.py"], check=True)
    end = time.time()
    return round(end - start, 4)

def run_ocaml():
    print("Compiling OCaml code...")
    # Compile OCaml code
    compile_cmd = ["ocamlopt", "-o", "ocaml/sum_parallel", "-I", "+domainslib", "domainslib.cmxa", "ocaml/sum_parallel.ml"]
    subprocess.run(compile_cmd, check=True)

    print("Running OCaml version...")
    start = time.time()
    subprocess.run(["./ocaml/sum_parallel"], check=True)
    end = time.time()
    return round(end - start, 4)

def main():
    os.makedirs("results", exist_ok=True)

    timings = {
        "problem": "parallel_sum",
        "input_size": 10_000_000,
        "results": []
    }

    try:
        ocaml_time = run_ocaml()
        timings["results"].append({
            "language": "OCaml",
            "time_seconds": ocaml_time
        })
    except subprocess.CalledProcessError:
        print("Error compiling/running OCaml.")

    try:
        python_time = run_python()
        timings["results"].append({
            "language": "Python",
            "time_seconds": python_time
        })
    except subprocess.CalledProcessError:
        print("Error running Python.")

    with open(RESULTS_PATH, "w") as f:
        json.dump(timings, f, indent=2)
        print(f"\nBenchmark saved to {RESULTS_PATH}")

if __name__ == "__main__":
    main()
