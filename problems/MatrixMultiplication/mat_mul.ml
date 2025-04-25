(* This file contains the code of all the functions used in multiplying the matrices - all in one file. Additionally, gets the time before executing the instructions in order to compare the runtimes of different function calls*)

let random_int min max =
        min + (Random.int (max-min+1))

let create_random_matrix rows cols =
        let random_num = random_int 0 100 in
        Array.init rows (fun _ ->
                Array.init cols ( fun _ -> random_num )
        )

let transpose m =
        let rows = Array.length m in
        let cols = Array.length m.(0) in
        Array.init cols (fun j -> Array.init rows (fun i -> m.(i).(j)))

let matrix_mult a b =
        let rows_a = Array.length a
        and cols_a = Array.length a.(0)
        and rows_b = Array.length b
        and cols_b = Array.length b.(0) in
        if cols_a <> rows_b then
                failwith "Matrix dimensions do not match for multiplication";
        Array.init rows_a (fun i ->
                Array.init cols_b (fun j ->
                        let sum=ref 0 in
                        for k=0 to cols_a -1 do
                                sum:= !sum + (a.(i).(k) * b.(k).(j))
                        done;
                        !sum
                )
        )

let dot_product a b =
        let len = Array.length a in
        let sum = ref 0 in
        for i=0 to len-1 do
                sum:= !sum + (a.(i) * b.(i))
        done;
        !sum

let matrix_mult_partial_eval a b =
        let b_t = transpose b in
        let rows_a = Array.length a
        and cols_b = Array.length b_t in
        Array.init rows_a (fun i ->
                let partial_prod = dot_product a.(i) in
                Array.init cols_b (fun j ->
                        partial_prod b_t.(j)
                )
        )

let matrix_mult_partial_eval_parallel a b=
        let b_t = transpose b in
        let rows_a = Array.length a
        and cols_b = Array.length b_t in
        let result = Array.make_matrix rows_a cols_b 0 in

        let compute_row i=
                let partial_dot_prod = dot_product a.(i) in
                for j=0 to cols_b - 1 do
                        result.(i).(j) <- partial_dot_prod b_t.(j)
                done
        in

        let domains = Array.init rows_a (fun i -> Domain.spawn (fun () -> compute_row i)) in
        Array.iter Domain.join domains;
        result

let time_it f =
        let start = Unix.gettimeofday () in
        let result = f () in
        let finish = Unix.gettimeofday () in
        Printf.printf "Time taken: %.6f seconds\n%!" (finish -. start);
        result

let () =
        Random.self_init ();
        let rows_a, cols_a = 70, 80 in
        let rows_b, cols_b = 80, 90 in

        Printf.printf "Creating random matrices of size %dx%d and %dx%d...\n%!" rows_a cols_a rows_b cols_b;
        let matrix_a = create_random_matrix rows_a cols_a in
        let matrix_b = create_random_matrix rows_b cols_b in

        Printf.printf "\nRunning matrix_mult...\n%!";
        let _ = time_it (fun () -> matrix_mult matrix_a matrix_b) in

        Printf.printf "\nRunning matrix_mult_partial_eval...\n%!";
        let _ = time_it (fun () -> matrix_mult_partial_eval matrix_a matrix_b) in

        Printf.printf "\nRunning matrix_mult_partial_eval_parallel...\n%!";
        let _ = time_it (fun () -> matrix_mult_partial_eval_parallel matrix_a matrix_b) in
        ()
