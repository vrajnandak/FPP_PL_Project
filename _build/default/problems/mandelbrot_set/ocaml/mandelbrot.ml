(* open Domainslib

let width = 100
let height = 3600
let zoom = 600.0
let max_iter = 2000

(* Higher-order function: takes a pixel coordinate -> returns iterations *)
let mandelbrot_iter x y =
  let rec loop zx zy i =
    if i >= max_iter then i
    else
      let zx2 = zx *. zx and zy2 = zy *. zy in
      if zx2 +. zy2 > 4.0 then i
      else loop (zx2 -. zy2 +. x) (2.0 *. zx *. zy +. y) (i + 1)
  in
  loop 0.0 0.0 0

(* Lazy evaluation: each row is lazily computed *)
let compute_row row =
  Lazy.force (
    Lazy.from_fun (fun () ->
      Array.init width (fun col ->
        let x = (float_of_int col /. zoom) -. 2.0 in
        let y = (float_of_int row /. zoom) -. 1.5 in
        mandelbrot_iter x y))
  )

let () =
  let pool = Task.setup_pool ~num_domains:4 () in
  let output = Task.run pool (fun () ->
    Array.init height (fun row ->
      Task.async pool (fun () -> compute_row row)
    )
    |> Array.map (Task.await pool)
  ) in
  Task.teardown_pool pool;

  (* Persistent structure: immutable array-of-arrays *)
  let oc = open_out "mandelbrot_output_new.csv" in
  Array.iter (fun row ->
    Array.iteri (fun i iter ->
      Printf.fprintf oc "%d%s" iter (if i = width - 1 then "\n" else ",")
    ) row
  ) output;
  close_out oc;
  print_endline "Parallel OCaml Mandelbrot set written to mandelbrot_output.csv" *)






  (* CORRECT CODE *)
  open Domainslib

  let width = 800
  let height = 600
  let zoom = 300.0
  let max_iter = 1000
  
  (* Higher-order function: takes a pixel coordinate -> returns iterations *)
  let mandelbrot_iter x y =
    let rec loop zx zy i =
      if i >= max_iter then i
      else
        let zx2 = zx *. zx and zy2 = zy *. zy in
        if zx2 +. zy2 > 4.0 then i
        else loop (zx2 -. zy2 +. x) (2.0 *. zx *. zy +. y) (i + 1)
    in
    loop 0.0 0.0 0
  
  (* Lazy evaluation: each row is lazily computed *)
  let compute_row row =
    Lazy.force (
      Lazy.from_fun (fun () ->
        Array.init width (fun col ->
          let x = (float_of_int col /. zoom) -. 2.0 in
          let y = (float_of_int row /. zoom) -. 1.5 in
          mandelbrot_iter x y))
    )
  
  let () =
    let pool = Task.setup_pool ~num_domains:4 () in
    let output = Task.run pool (fun () ->
      Array.init height (fun row ->
        Task.async pool (fun () -> compute_row row)
      )
      |> Array.map (Task.await pool)
    ) in
    Task.teardown_pool pool;
  
    (* Persistent structure: immutable array-of-arrays *)
    let oc = open_out "mandelbrot_output.csv" in
    Array.iter (fun row ->
      Array.iteri (fun i iter ->
        Printf.fprintf oc "%d%s" iter (if i = width - 1 then "\n" else ",")
      ) row
    ) output;
    close_out oc;
    print_endline "Parallel OCaml Mandelbrot set written to mandelbrot_output.csv"



    (* open Domainslib

    let width = 800
    let height = 600
    let zoom = 300.0
    let inv_zoom = 1.0 /. zoom
    let max_iter = 1000
    
    (* GC tuning for performance *)
    let () =
      Gc.set {
        (Gc.get ()) with
        minor_heap_size = 1_000_000;
        space_overhead = 80;
      }
    
    (* Higher-order function: takes a pixel coordinate -> returns iterations *)
    let mandelbrot_iter x y =
      let rec loop zx zy i =
        if i >= max_iter then i
        else
          let zx2 = zx *. zx and zy2 = zy *. zy in
          if zx2 +. zy2 > 4.0 then i
          else loop (zx2 -. zy2 +. x) (2.0 *. zx *. zy +. y) (i + 1)
      in
      loop 0.0 0.0 0
    
    (* Lazy evaluation: each row is lazily computed *)
    let compute_row row =
      Lazy.force (
        Lazy.from_fun (fun () ->
          let arr = Array.make width 0 in
          for col = 0 to width - 1 do
            let x = (float col *. inv_zoom) -. 2.0 in
            let y = (float row *. inv_zoom) -. 1.5 in
            Array.unsafe_set arr col (mandelbrot_iter x y)
          done;
          arr
        )
      )
    
    let () =
      let pool = Task.setup_pool ~num_domains:4 () in
      let output = Task.run pool (fun () ->
        Array.init height (fun row ->
          Task.async pool (fun () -> compute_row row)
        )
        |> Array.map (Task.await pool)
      ) in
      Task.teardown_pool pool;
    
      let oc = open_out "mandelbrot_output.csv" in
      Array.iter (fun row ->
        Array.iteri (fun i iter ->
          Printf.fprintf oc "%d%s" iter (if i = width - 1 then "\n" else ",")
        ) row
      ) output;
      close_out oc;
      print_endline "Parallel OCaml Mandelbrot set written to mandelbrot_output.csv" *)
    


