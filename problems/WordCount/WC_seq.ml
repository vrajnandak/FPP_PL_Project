let split_words_to_array str =
  let regexp = Str.regexp "[^a-zA-Z]+" in
  Str.split regexp str |> Array.of_list

let count_words_array arr =
  let counts = Hashtbl.create 100 in
  Array.iter (fun word ->
    let word = String.lowercase_ascii word in
    let count = try Hashtbl.find counts word with Not_found -> 0 in
    Hashtbl.replace counts word (count + 1)
  ) arr;
  counts

let () =
  let total_start = Unix.gettimeofday () in

  (* File read and splitting *)
  (* let file_read_start = Unix.gettimeofday () in *)
  let all_words =
    let filename = "input.txt" in
    let ic = open_in filename in
    let len = in_channel_length ic in
    let text = really_input_string ic len in
    close_in ic;
    split_words_to_array text
  in
  (* let file_read_end = Unix.gettimeofday () in *)

  (* Word counting computation *)
  (* let compute_start = Unix.gettimeofday () in *)
  let word_counts = count_words_array all_words in
  (* let compute_end = Unix.gettimeofday () in *)

  (* Final timings *)
  let total_end = Unix.gettimeofday () in

  (* Printf.printf "\nFile Read + Split Time     : %.6f seconds\n" (file_read_end -. file_read_start); *)
  (* Printf.printf "Computation Time (Counting): %.6f seconds\n" (compute_end -. compute_start); *)
  Printf.printf "Total Execution Time       : %.6f seconds\n" (total_end -. total_start)
