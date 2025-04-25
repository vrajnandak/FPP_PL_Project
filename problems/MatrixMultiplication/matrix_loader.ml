let load_matrix filename =
        let ic = open_in_bin filename in
        let matrix = Marshal.from_channel ic in
        close_in ic;
        matrix

