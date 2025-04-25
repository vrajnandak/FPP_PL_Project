    (* open Mpi

let get_cpu () =
  let ic = Unix.open_process_in "taskset -pc $$" in
  let line = input_line ic in
  close_in ic;
  line


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

let aggregate_counts src dst =
  Hashtbl.iter (fun key value ->
    let count = try Hashtbl.find dst key with Not_found -> 0 in
    Hashtbl.replace dst key (count + value)
  ) src

let read_file filename =
  let ic = open_in filename in
  let n = in_channel_length ic in
  let s = really_input_string ic n in
  close_in ic;
  s

let () =
  let rank = Mpi.comm_rank Mpi.comm_world in
  let size = Mpi.comm_size Mpi.comm_world in

  let start_time = Unix.gettimeofday () in

  let file_read_start = Unix.gettimeofday () in

  let all_words =
    if rank = 0 then
      let text = read_file "input.txt" in
      split_words_to_array text
    else
      [||]
  in

  let file_read_end = Unix.gettimeofday () in

  Printf.printf "File Read     : %.6f seconds\n" (file_read_end -. file_read_start);

  let split_bcast_start = Unix.gettimeofday () in

  let all_words = Mpi.broadcast all_words 0 Mpi.comm_world in

  let total_words = Array.length all_words in
  let words_per_proc = total_words / size in
  let start_index = rank * words_per_proc in
  let end_index = if rank = size - 1 then total_words else (rank + 1) * words_per_proc in
  let chunk_size = end_index - start_index in
  let chunk = Array.sub all_words start_index chunk_size in

  (* Printf.printf "Process %d is running on: %s\n%!" rank (get_cpu ()); *)

  let split_bcast_end = Unix.gettimeofday () in

  Printf.printf "Broadcast     : %.6f seconds\n" (split_bcast_end -. split_bcast_start);


  let computation_start_time = Unix.gettimeofday () in
  let local_counts = count_words_array chunk in
  let computation_end_time = Unix.gettimeofday () in
  let computation_time = computation_end_time -. computation_start_time in

  Printf.printf "Process %d Computation Time  : %.6f seconds\n" rank computation_time;

  let communication_start_time = Unix.gettimeofday () in
  if rank <> 0 then
    Mpi.send (Hashtbl.to_seq local_counts |> Array.of_seq) 0 0 Mpi.comm_world
  else begin
    let master_counts = Hashtbl.copy local_counts in
    for i = 1 to size - 1 do
      let partial_seq = Mpi.receive i 0 Mpi.comm_world in
      let partial_counts = Hashtbl.of_seq (Array.to_seq partial_seq) in
      aggregate_counts partial_counts master_counts
    done;

    let communication_end_time = Unix.gettimeofday () in
    let communication_time = communication_end_time -. communication_start_time in
    Printf.printf "Process %d Communication Time: %.6f seconds\n!" rank communication_time;
    
    let end_time = Unix.gettimeofday () in
    (* let chunk_str = String.concat " " (Array.to_list chunk) in *)
    (* Printf.printf "Process %d chunk: %s\n%!" rank chunk_str; *)
    
    (* Printf.printf "\nFinal Word Counts:\n";
    Hashtbl.iter (fun word count ->
      Printf.printf "%s: %d\n" word count
    ) master_counts; *)

    (* Printf.printf "\nTotal Unique Words: %d\n" (Hashtbl.length master_counts); *)

    Printf.printf "Execution Time: %.6f seconds\n" (end_time -. start_time);
  end; *)


open Mpi

let get_cpu () =
  let ic = Unix.open_process_in "taskset -pc $$" in
  let line = input_line ic in
  close_in ic;
  line

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

let aggregate_counts src dst =
  Hashtbl.iter (fun key value ->
    let count = try Hashtbl.find dst key with Not_found -> 0 in
    Hashtbl.replace dst key (count + value)
  ) src

let read_file filename =
  let ic = open_in filename in
  let n = in_channel_length ic in
  let s = really_input_string ic n in
  close_in ic;
  s

let () =
  let rank = Mpi.comm_rank Mpi.comm_world in
  let size = Mpi.comm_size Mpi.comm_world in

  let start_time = Unix.gettimeofday () in

  let file_read_start = Unix.gettimeofday () in

  let all_words =
    if rank = 0 then
      let text = read_file "input.txt" in
      split_words_to_array text
    else
      [||]
  in

  let file_read_end = Unix.gettimeofday () in

  Printf.printf "Process %d File Read     : %.6f seconds\n%!" rank (file_read_end -. file_read_start);

  let bcast_start = Unix.gettimeofday () in
  let all_words = Mpi.broadcast all_words 0 Mpi.comm_world in
  let bcast_end = Unix.gettimeofday () in

  Printf.printf "Process %d Broadcast     : %.6f seconds\n%!" rank (bcast_end -. bcast_start);

  let split_start = Unix.gettimeofday () in

  let total_words = Array.length all_words in
  let words_per_proc = total_words / size in
  let start_index = rank * words_per_proc in
  let end_index = if rank = size - 1 then total_words else (rank + 1) * words_per_proc in
  let chunk_size = end_index - start_index in
  let chunk = Array.sub all_words start_index chunk_size in
  
  let split_end = Unix.gettimeofday () in
  Printf.printf "Process %d Split         : %.6f seconds\n%!" rank (split_end -. split_start);


  let computation_start_time = Unix.gettimeofday () in
  let local_counts = count_words_array chunk in
  let computation_end_time = Unix.gettimeofday () in
  let computation_time = computation_end_time -. computation_start_time in

  Printf.printf "Process %d Computation Time  : %.6f seconds\n%!" rank computation_time;

  let communication_start_time = Unix.gettimeofday () in

  if rank <> 0 then
    Mpi.send (Hashtbl.to_seq local_counts |> Array.of_seq) 0 0 Mpi.comm_world
  else begin
    let master_counts = Hashtbl.copy local_counts in
    for i = 1 to size - 1 do
      let partial_seq = Mpi.receive i 0 Mpi.comm_world in
      let partial_counts = Hashtbl.of_seq (Array.to_seq partial_seq) in
      aggregate_counts partial_counts master_counts
    done;

    let communication_end_time = Unix.gettimeofday () in
    let communication_time = communication_end_time -. communication_start_time in
    Printf.printf "Process %d Communication Time: %.6f seconds\n%!" rank communication_time;

    let end_time = Unix.gettimeofday () in
    Printf.printf "Process %d Execution Time: %.6f seconds\n%!" rank (end_time -. start_time);
  end
