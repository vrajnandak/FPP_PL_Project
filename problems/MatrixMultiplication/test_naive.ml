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

let ()=
        let a = Matrix_loader.load_matrix "matrix_a.bin" in
        let b = Matrix_loader.load_matrix "matrix_b.bin" in
        ignore (matrix_mult a b)
