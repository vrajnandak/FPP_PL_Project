open Domainslib

let parallel_sum arr =
  let pool = Task.setup_pool ~num_additional_domains:(Domain.recommended_domain_count () - 1) () in
  let len = Array.length arr in
  let chunk_size = len / Domain.recommended_domain_count () in
  let sum =
    Task.parallel_for_reduce
      ~start:0
      ~finish:(len - 1)
      ~body:(fun i -> arr.(i))
      ~pool
      ~chunk_size
      ~reduce:( + )
      ~neutral:0
  in
  Task.teardown_pool pool;
  sum

let () =
  let n = 10_000_000 in
  let arr = Array.init n (fun i -> i + 1) in
  let t0 = Sys.time () in
  let result = parallel_sum arr in
  let t1 = Sys.time () in
  Printf.printf "Sum = %d\n" result;
  Printf.printf "Time taken (OCaml) = %.4f seconds\n" (t1 -. t0)
