let transpose m =
        let rows = Array.length m in
        let cols = Array.length m.(0) in
        Array.init cols (fun j -> Array.init rows (fun i -> m.(i).(j)))

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
        (*
        if cols_a <> rows_b then
                failwith "Matrix dimensions do not match for multiplication";
                *)
        Array.init rows_a (fun i ->
                let partial_prod = dot_product a.(i) in
                Array.init cols_b (fun j ->
                        (*partial_prod (Array.init rows_b (fun k -> b.(k).(j))) *)
                        (*dot_product b.(j)*)
                        partial_prod b_t.(j)
                )
        )

let () =
        let a = Matrix_loader.load_matrix "matrix_a.bin" in
        let b = Matrix_loader.load_matrix "matrix_b.bin" in
        ignore (matrix_mult_partial_eval a b)
