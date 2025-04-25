let random_int min max =
        min + (Random.int (max-min+1))

let create_random_matrix rows cols =
        let random_num = random_int 0 100 in
        Array.init rows (fun _ -> 
                Array.init cols ( fun _ -> random_num )
        )

let save_matrix filename matrix =
        let oc = open_out_bin filename in
        Marshal.to_channel oc matrix [];
        close_out oc

let () =
        Random.init 42;
        let m1 = create_random_matrix 50 60 in
        let m2 = create_random_matrix 60 70 in
        save_matrix "matrix_a.bin" m1;
        save_matrix "matrix_b.bin" m2;
        print_endline "Matrices saved to matrix_a.bin and matrix_b.bin"
