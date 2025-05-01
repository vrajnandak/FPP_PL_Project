open Parmap

type state = Susceptible | Infected | Recovered

type person = {
  mutable state: state;
}

type population = person array array

let step beta gamma (population : population) =
  let rows = Array.length population in
  let cols = Array.length population.(0) in
  for i = 0 to rows - 1 do
    for j = 0 to cols - 1 do
      let person = population.(i).(j) in
      match person.state with
      | Susceptible ->
          let infected_neighbors = ref 0 in
          for di = -1 to 1 do
            for dj = -1 to 1 do
              if not (di = 0 && dj = 0) then
                let ni = i + di in
                let nj = j + dj in
                if ni >= 0 && ni < rows && nj >= 0 && nj < cols then
                  if population.(ni).(nj).state = Infected then
                    infected_neighbors := !infected_neighbors+1
            done
          done;
          let prob = 1.0 -. (1.0 -. beta) ** float_of_int !infected_neighbors in
          if Random.float 1.0 < prob then
            person.state <- Infected
      | Infected ->
          if Random.float 1.0 < gamma then
            person.state <- Recovered
      | Recovered -> ()
    done
  done


let simulate beta gamma rows cols steps =
  let pop = Array.init rows (fun _ ->
              Array.init cols (fun _ -> { state = Susceptible })
            )
  in
  pop.(rows/2).(cols/2).state <- Infected;
  for _ = 1 to steps do
    step beta gamma pop
  done;
  Array.fold_left (fun acc row ->
    acc + Array.fold_left (fun acc p ->
      if p.state = Infected then acc + 1 else acc
    ) 0 row
  ) 0 pop

let () =
  Random.self_init ();
  let runs = 1000 in
  let beta = 0.2 in
  let gamma = 0.1 in
  let rows = 50 in
  let cols = 50 in
  let steps = 50 in


  let t1 = Unix.gettimeofday () in
  let infections =
    Parmap.parmap ~ncores:4
      (fun _ -> simulate beta gamma rows cols steps)
      (Parmap.A (Array.init runs (fun i -> i)))
  in
  let t2 = Unix.gettimeofday () in


  List.iteri (fun i inf ->
    Printf.printf "Simulation %d: %d infections\n" (i+1) inf
  ) infections;

  let avg =
    (float_of_int (Array.fold_left (+) 0 (Array.of_list infections))) /. float_of_int runs
  in
  Printf.printf "\nParallel: Time taken = %.4f seconds\n" (t2 -. t1);
  Printf.printf "Average infections at end of simulation: %.2f\n" avg
  