(* Function to create a random matrix of given size whose elements are less than 100 *)
let random_int min max =
        min + (Random.int (max-min+1))

let create_random_matrix rows cols =
        let random_num = random_int 0 100 in
        Array.init rows (fun _ -> 
                Array.init cols ( fun _ -> random_num )
        )

(* Function to write the resultant product matrix to a file*)
let write_to_file filename m =
        let rows = Array.length m in
        let cols = Array.length m.(0) in
        let oc = open_out filename in
        Printf.fprintf oc "%d %d\n" rows cols;
        Array.iter (fun row ->                                  (* iterating for every row of the matrix *)
                Array.iter (fun col ->                          (* iterating for every value in the row *)
                        Printf.fprintf oc "%d " col
                ) row;
                output_char oc '\n'
        ) m;
        close_out oc

(* Function to read a matrix from the given file as an int array array datatype and return it *)
let read_matrix filename =
        let ic = open_in filename in
        try
                let line = input_line ic in                                     (* reading the first line of the matrix file - number of rows, cols *)
                let rows, cols =
                        match String.split_on_char ' ' line |> List.map int_of_string with
                        | [r;c] -> (r,c)
                        | _ -> failwith "Invalid matrix size line at the start of the file"
                in
                let matrix = Array.init rows (fun _ ->                          (* creating an array with 'rows' number of rows *)
                        let row_line = input_line ic in                         (* reading the input line - is an entire row of the matrix *)
                        row_line 
                        |> String.split_on_char ' '                             (* is split on ' ' *)
                        |> List.filter (fun s -> String.trim s <> "")           (* discarding all the empty strings after the split if any *)
                        |> List.map int_of_string                               (* converting the string representation of the numbers to int *)
                        |> Array.of_list                                        (* storing the list as an array for efficiency purposes *)
                ) in
                close_in ic;
                matrix
        with e ->
                close_in_noerr ic;
                raise e

(*********************** SEQUENTIAL MATRIX MULTIPLICATION ****************************)

let matrix_mult a b =
        let rows_a = Array.length a
        and cols_a = Array.length a.(0)
        and rows_b = Array.length b
        and cols_b = Array.length b.(0) in
        if cols_a <> rows_b then
                failwith "Matrix dimensions do not match for multiplication";

        (* creating the resultant product matrix using c[i][j] = sigma on 'k' (a[i][k] * b[k][j]) *)
        Array.init rows_a (fun i ->
                Array.init cols_b (fun j ->
                        let sum = ref 0 in
                        for k = 0 to cols_a - 1 do
                                sum:= !sum + (a.(i).(k) * b.(k).(j))
                        done;
                        !sum
                )
        )



(*********************** PARTIAL EVALUATION FOR MATRIX MULTIPLICATION ****************************)
let transpose m =
        let rows = Array.length m in
        let cols = Array.length m.(0) in
        Array.init cols (fun j -> Array.init rows (fun i -> m.(i).(j)))         (* places element a[i][j] at a[j][i] *)

let dot_product a b =
        let len = Array.length a in
        let sum = ref 0 in
        for i=0 to len-1 do
                sum:= !sum + (a.(i) * b.(i))
        done;
        !sum

let matrix_mult_partial_eval a b =                                      (* computes product using b_transpose for efficiency *)
        let b_t = transpose b in
        let rows_a = Array.length a
        and cols_b = Array.length b_t in
        Array.init rows_a (fun i ->                                     (* iterates on every row of the left matrix in the multiplication *)
                let partial_prod = dot_product a.(i) in                 (* partially evaluating dot product with a[i] due to repeated usage*)
                Array.init cols_b (fun j ->
                        partial_prod b_t.(j)                            (* calling the partially evaluated dot product on jth row of b transpose, corresponding to the jth column of b *)
                )
        )

(*********************** PARTIAL EVALUATION AND PARALLELISED (only domains) MATRIX MULTIPLICATION *******************)
let matrix_mult_partial_eval_parallel a b=
        let b_t = transpose b in
        let rows_a = Array.length a
        and cols_b = Array.length b_t in

        let result = Array.make_matrix rows_a cols_b 0 in

        let compute_row i=
                let partial_dot_prod = dot_product a.(i) in             (* partially evaluating dot product with a[i] due to repeated usage *)
                for j=0 to cols_b - 1 do
                        result.(i).(j) <- partial_dot_prod b_t.(j)      (* calling the partially evaluated dot product on jth row of b transpose *)
                done
        in

        let domains = Array.init rows_a (fun i -> Domain.spawn (fun () -> compute_row i)) in            (* creating an array of domains to run the computation for getting a row in the resultant product *)
        Array.iter Domain.join domains;                                 (* waiting for the domains to finish execution *)
        result

(*********************** PARTIAL EVALUATION AND PARALLELISED (domainslib lib) MATRIX MULTIPLICATION *******************)
module Task = Domainslib.Task
let matrix_mult_partial_eval_domainslib pool a b =

        let b_t = transpose b in
        let rows_a = Array.length a
        and cols_b = Array.length b_t in

        let result = Array.make_matrix rows_a cols_b 0 in

        (* parallelising to ensure each Task runs to compute an element in the resultant product matrix *)
        Task.parallel_for pool ~start:0 ~finish:(rows_a - 1)
        ~body:(fun i ->
                let partial_dot_prod = dot_product a.(i) in             (* partially evaluating dot product with a[i] due to repeated usage *)
                Task.parallel_for pool ~start:0 ~finish:(cols_b - 1)
                ~body: ( fun j ->
                        result.(i).(j) <- partial_dot_prod b_t.(j)      (* calling the partially evaluated dot product on jth row of b transpose *)
                )
        );

        result


(* MAIN ENTRY POINT *)
let () =
        if Array.length Sys.argv < 3 then 
        (
                Printf.eprintf "Usage: %s <arg> <shd_output>: arg2 can be 'seq', 'partial', 'partial_parallel', 'partial_parallel_domainslib'. arg3 can be 'yes' to write the outputs\n" Sys.argv.(0);
                exit 1
        )
        else
                let arg1 = Sys.argv.(1) in
                let arg2 = Sys.argv.(2) in
                
                let matrix_a = read_matrix "matrix_a.txt" in
                let matrix_b = read_matrix "matrix_b.txt" in

                if arg1 = "seq" then
                        let result = matrix_mult matrix_a matrix_b in
                        if arg2 = "yes" then
                                write_to_file "OCaml_mat_mul_seq.txt" result
                        else
                                ()
                else if arg1 = "partial" then
                        let result = matrix_mult_partial_eval matrix_a matrix_b in
                        if arg2 = "yes" then
                                write_to_file "OCaml_mat_mul_partial_eval.txt" result
                        else
                                ()
                else if arg1 = "partial_parallel" then
                        let result = matrix_mult_partial_eval_parallel matrix_a matrix_b in
                        if arg2 = "yes" then
                                write_to_file "OCaml_mat_mul_partial_eval_parallel.txt" result
                        else
                                ()
                else if arg1 = "partial_parallel_domainslib" then
                        let pool = Task.setup_pool ~num_domains:3 () in
                        let result = Task.run pool (fun _ -> matrix_mult_partial_eval_domainslib pool matrix_a matrix_b) in
                        Task.teardown_pool pool;
                        if arg2 = "yes" then
                                write_to_file "OCaml_mat_mul_partial_eval_parallel_domainslib.txt" result
                        else
                                ()
                else
                        Printf.eprintf "Usage: %s <arg> <shd_output>: arg2 can be 'seq', 'partial', 'partial_parallel', 'partial_parallel_domainslib'. arg3 can be 'yes' to write the outputs\n" Sys.argv.(0);
                        ()
