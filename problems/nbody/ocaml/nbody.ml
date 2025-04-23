open Spawn

let g = 6.67430e-11
let dt = 0.01

type body = {
  id : int;
  mutable x : float;
  mutable y : float;
  mutable vx : float;
  mutable vy : float;
  mass : float;
}

type msg =
  | ComputeForces of (int * float * float * float) list  (* id, x, y, mass *)
  | UpdatePosition

let distance dx dy = sqrt (dx *. dx +. dy *. dy +. 1e-10)

let body_actor (self : (msg, unit) Spawn.t) (body : body) =
  let rec loop () =
    receive self (function
      | ComputeForces bodies ->
          let fx = ref 0.0 in
          let fy = ref 0.0 in
          List.iter (fun (j, xj, yj, mj) ->
            if j <> body.id then (
              let dx = xj -. body.x in
              let dy = yj -. body.y in
              let r = distance dx dy in
              let f = g *. body.mass *. mj /. (r *. r) in
              fx := !fx +. f *. dx /. r;
              fy := !fy +. f *. dy /. r;
            )
          ) bodies;

          let ax = !fx /. body.mass in
          let ay = !fy /. body.mass in
          body.vx <- body.vx +. ax *. dt;
          body.vy <- body.vy +. ay *. dt;
          recur loop

      | UpdatePosition ->
          body.x <- body.x +. body.vx *. dt;
          body.y <- body.y +. body.vy *. dt;
          recur loop
    )
  in
  loop ()

let main () =
  let n = 10 in
  let steps = 100 in

  let bodies =
    Array.init n (fun i ->
      {
        id = i;
        x = float_of_int (100 * i);
        y = float_of_int (100 * i);
        vx = 0.0;
        vy = 0.0;
        mass = 1e3;
      })
  in

  run (fun () ->
    let actors =
      Array.map (fun b -> spawn (fun self -> body_actor self b)) bodies
    in

    for _t = 1 to steps do
      let snapshot =
        Array.to_list bodies
        |> List.map (fun b -> (b.id, b.x, b.y, b.mass))
      in

      Array.iter (fun a -> send a (ComputeForces snapshot)) actors;
      Array.iter (fun a -> send a UpdatePosition) actors
    done;

    Printf.printf "Simulation done.\n";
    Array.iteri (fun i b ->
      Printf.printf "Body %d: x = %.2f, y = %.2f\n" i b.x b.y
    ) bodies
  )

let () = main ()
