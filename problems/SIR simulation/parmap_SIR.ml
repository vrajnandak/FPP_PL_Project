open Parmap

type state = Susceptible | Infected | Recovered

type person = {
  mutable state: state;
}

type population = person option array array

let state_to_char = function
  | Susceptible -> 'S'
  | Infected -> 'I'
  | Recovered -> 'R'

let print_population population =
  Array.iter (fun row ->
    Array.iter (fun cell ->
      match cell with
      | Some p -> Printf.printf "%c " (state_to_char p.state)
      | None -> Printf.printf ". "
    ) row;
    print_newline ()
  ) population

let initialize_population rows cols density rand =
  Array.init rows (fun _ ->
    Array.init cols (fun _ ->
      if Random.State.float rand 1.0 < density then Some { state = Susceptible } else None
    )
  )

let shuffle_array arr rand =
  let n = Array.length arr in
  for i = n - 1 downto 1 do
    let j = Random.State.int rand (i + 1) in
    let tmp = arr.(i) in
    arr.(i) <- arr.(j);
    arr.(j) <- tmp
  done

let move_population ?(move_prob = 0.5) population rand =
  let rows = Array.length population in
  let cols = Array.length population.(0) in
  let directions = [|(0, 1); (1, 0); (0, -1); (-1, 0)|] in

  let coords = Array.init (rows * cols) (fun k -> (k / cols, k mod cols)) in
  shuffle_array coords rand;

  Array.iter (fun (i, j) ->
    match population.(i).(j) with
    | Some person ->
        if Random.State.float rand 1.0 < move_prob then
          let (di, dj) = directions.(Random.State.int rand 4) in
          let ni, nj = i + di, j + dj in
          if ni >= 0 && ni < rows && nj >= 0 && nj < cols && population.(ni).(nj) = None then (
            population.(ni).(nj) <- Some person;
            population.(i).(j) <- None
          )
    | None -> ()
  ) coords;
  population

let step beta gamma population rand =
  let rows = Array.length population in
  let cols = Array.length population.(0) in
  let new_pop = Array.init rows (fun i ->
    Array.init cols (fun j ->
      match population.(i).(j) with
      | Some p -> Some { state = p.state }
      | None -> None
    )
  ) in

  for i = 0 to rows - 1 do
    for j = 0 to cols - 1 do
      match population.(i).(j) with
      | Some person ->
          let new_state =
            match person.state with
            | Susceptible ->
                let count = ref 0 in
                for di = -1 to 1 do
                  for dj = -1 to 1 do
                    let ni, nj = i + di, j + dj in
                    if (di != 0 || dj != 0) && ni >= 0 && ni < rows && nj >= 0 && nj < cols then
                      match population.(ni).(nj) with
                      | Some { state = Infected } -> incr count
                      | _ -> ()
                  done
                done;
                if !count > 0 then
                  let prob = 1.0 -. (1.0 -. beta) ** float !count in
                  if Random.State.float rand 1.0 < prob then Infected else Susceptible
                else Susceptible
            | Infected ->
                if Random.State.float rand 1.0 < gamma then Recovered else Infected
            | Recovered -> Recovered
          in
          (match new_pop.(i).(j) with
          | Some p -> p.state <- new_state
          | None -> ())
      | None -> ()
    done
  done;
  (* print_population new_pop;
  print_newline (); *)
  new_pop

let simulate beta gamma rows cols steps density run_id =
  let rand = Random.State.make [| run_id |] in
  let population = initialize_population rows cols density rand in

  let rec infect_one () =
    let i = Random.State.int rand rows in
    let j = Random.State.int rand cols in
    match population.(i).(j) with
    | Some p -> p.state <- Infected
    | None -> infect_one ()
  in
  infect_one ();

  let rec simulate_steps pop step_num =
    if step_num = 0 then pop
    else
      let next = step beta gamma pop rand in
      let moved = move_population next rand in
      simulate_steps moved (step_num - 1)
  in

  let final = simulate_steps population steps in
  Array.fold_left (fun acc row ->
    acc + Array.fold_left (fun acc cell ->
      match cell with
      | Some { state = Infected } -> acc + 1
      | _ -> acc
    ) 0 row
  ) 0 final

let () =
  Random.self_init ();
  let runs = 100 in
  let beta = 0.2 in
  let gamma = 0.05 in
  let rows, cols = 50, 50 in
  let steps = 50 in
  let density = 0.7 in

  let t1 = Unix.gettimeofday () in
  let infections =
    Parmap.parmap ~ncores:4
      (fun run_id -> simulate beta gamma rows cols steps density run_id)
      (Parmap.A (Array.init runs Fun.id))
  in
  let t2 = Unix.gettimeofday () in

  List.iteri (fun i inf ->
    Printf.printf "Simulation %d: %d infections\n%!" (i+1) inf
  ) infections;

  let avg = float (List.fold_left (+) 0 infections) /. float runs in
  Printf.printf "\nParallel: Time taken = %.4f seconds\n" (t2 -. t1);
  Printf.printf "Average infections at end of simulation: %.2f\n" avg
