  open Domainslib
(* Define image dimensions and computation parameters *)
  let width = 1600
  let height = 1200
  let zoom = 300.0
  let max_iter = 1000
  
  (* Higher-order function: takes a pixel coordinate -> returns iterations *)
  let mandelbrot_iter x y =
    let rec loop zx zy i =
      if i >= max_iter then i
      else
        let zx2 = zx *. zx in
        let zy2 = zy *. zy in
        let sum = zx2 +. zy2 in
        if sum > 4.0 then i     (* Escapes if magnitude exceeds threshold *)

        (* Recurse with new z values following the Mandelbrot formula *)
        else loop (zx2 -. zy2 +. x) (2.0 *. zx *. zy +. y) (i + 1)
    in
    loop 0.0 0.0 0
  
  (* Lazily computes a row of the Mandelbrot set image.
   Each pixel's iteration count is computed and stored in an array. *)
  let compute_row row =
    Lazy.force (
      Lazy.from_fun (fun () ->
        Array.init width (fun col ->
          let x = (float_of_int col /. zoom) -. 2.0 in
          let y = (float_of_int row /. zoom) -. 1.5 in
          mandelbrot_iter x y))
    )
  
    let () =
    (* Create a pool of 4 domains for parallel computation *)
    (* 4 was found to be the ideal size after contrasting with 2, 8, 12 domains *)
    let pool = Task.setup_pool ~num_domains:4 () in
    let t0 = Unix.gettimeofday () in
    (* Parallel computation of all rows using Domainslib task pool *)
    let output = Task.run pool (fun () ->
      Array.init height (fun row ->
        Task.async pool (fun () -> compute_row row)
      )
      |> Array.map (Task.await pool)
    ) in
    let t1 = Unix.gettimeofday () in
    (* Clean up the domain pool *)
    Task.teardown_pool pool;
  
    Printf.printf "Computation Time: %.4f seconds\n" (t1 -. t0);
  
    (* Write output *)
    let oc = open_out "mandelbrot_output.csv" in
    Array.iter (fun row ->
      Array.iteri (fun i iter ->
        Printf.fprintf oc "%d%s" iter (if i = width - 1 then "\n" else ",")
      ) row
    ) output;
    close_out oc;
  
    print_endline "Parallel OCaml Mandelbrot set written to mandelbrot_output.csv"


