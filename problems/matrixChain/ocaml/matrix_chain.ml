open Lwt.Infix
(* open Unix *)

(* Standard Sequential MCM *)
let matrix_chain_order_seq p =
  let n = Array.length p - 1 in
  if n <= 0 then 0
  else
    let m = Array.make_matrix (n + 1) (n + 1) 0 in
    for len = 2 to n do
      for i = 1 to n - len + 1 do
        let j = i + len - 1 in
        m.(i).(j) <- max_int;
        for k = i to j - 1 do
          let cost = Int64.(
            add (add (of_int m.(i).(k)) (of_int m.(k+1).(j)))
                (mul (mul (of_int p.(i-1)) (of_int p.(k))) (of_int p.(j)))
          ) in
          let int_cost =
            if cost > Int64.of_int max_int then max_int
            else Int64.to_int cost
          in
          if int_cost < m.(i).(j) then
            m.(i).(j) <- int_cost
        done
      done
    done;
    m.(1).(n)

(* Parallel MCM using Lwt *)
let matrix_chain_order_lwt p =
  let n = Array.length p - 1 in
  if n <= 0 then Lwt.return 0
  else
    let m = Array.make_matrix (n + 1) (n + 1) 0 in

    let compute_entry i j =
      let min_cost = ref max_int in
      for k = i to j - 1 do
        let cost = Int64.(
          add (add (of_int m.(i).(k)) (of_int m.(k+1).(j)))
              (mul (mul (of_int p.(i-1)) (of_int p.(k))) (of_int p.(j)))
        ) in
        let int_cost =
          if cost > Int64.of_int max_int then max_int
          else Int64.to_int cost
        in
        if int_cost < !min_cost then
          min_cost := int_cost
      done;
      m.(i).(j) <- !min_cost;
      Lwt.return ()
    in

    let rec loop len =
      if len > n then
        Lwt.return m.(1).(n)
      else
        let tasks = ref [] in
        for i = 1 to n - len + 1 do
          let j = i + len - 1 in
          tasks := compute_entry i j :: !tasks
        done;
        Lwt.join !tasks >>= fun () ->
        loop (len + 1)
    in
    loop 2

(* Helper to generate random dimensions *)
let generate_random_dimensions n ~min_dim ~max_dim seed =
  Random.init seed;
  Array.init (n + 1) (fun _ -> min_dim + Random.int (max_dim - min_dim + 1))

(* Timing helper *)
let time f =
  let t1 = Unix.gettimeofday () in
  let res = f () in
  let t2 = Unix.gettimeofday () in
  Printf.printf "Time: %.4fs\n" (t2 -. t1);
  res

(* Async timing helper *)
let time_lwt f =
  let t1 = Unix.gettimeofday () in
  f () >>= fun res ->
  let t2 = Unix.gettimeofday () in
  Printf.printf "Time: %.4fs\n" (t2 -. t1);
  Lwt.return res

(* Example Usage *)
let () =
  let num_matrices = 200 in
  let seed = 42 in
  let p = generate_random_dimensions num_matrices ~min_dim:5 ~max_dim:100 seed in
  Printf.printf "Generated %d matrices (dimensions array size: %d)\n"
    num_matrices (Array.length p);

  Printf.printf "Running sequential version...\n";
  let cost_seq = time (fun () -> matrix_chain_order_seq p) in
  Printf.printf "Sequential Min Cost: %d\n\n" cost_seq;

  Printf.printf "Running parallel (Lwt) version...\n";
  let cost_lwt = Lwt_main.run (
    time_lwt (fun () -> matrix_chain_order_lwt p)
  ) in
  Printf.printf "Lwt Min Cost: %d\n" cost_lwt
