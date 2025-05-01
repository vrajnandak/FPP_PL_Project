open Lwt.Infix

(* data type of the self-defined actor created for each node in the graph *)
type actor = {
        id: int;                                                        (* corresponds to node id in graph *)
        mutable state : float;                                          (* corresponds to state of node *)
        mutable prev_state : float;                                     (* for checking stabilisation of graph *)
        mutable send_tos : ((float option -> unit) * float) list;       (* the message channels to which the actor can push values *)
        mutable recv_froms : float Lwt_stream.t list;                   (* the message channels from which actor gets values *)
}


(* function for an actor to send it's state information to all it's outgoing edges *)
let send_state actor =
        List.iter (fun (push, edge_prob) ->
                push (Some (actor.state *. edge_prob))
        ) actor.send_tos;
        Lwt.return_unit

(* function for an actor to read state information values from all of it's nodes directed to it *)
let receive_and_update actor =
        let waits = List.map Lwt_stream.next actor.recv_froms in
        actor.state <- 0.;
        Lwt.all waits >|= fun xs ->
                let total=List.fold_left ( +. ) 0.0 xs in
                actor.state <- actor.state +. total

(* function to generate a random graph with 'n_nodes' nodes and 'n_edges' edges along with edge weights *)
let read_graph () =
        let ic = open_in "graph.txt" in
        let n_nodes = int_of_string (input_line ic) in
        let adj_out = Array.make n_nodes [] in
        for i=0 to n_nodes - 1 do
                let line = input_line ic in
                let parts = String.split_on_char ' ' line |> List.filter (fun s -> s <> "") in
                match parts with
                | num_edges_str :: rest ->
                                let num_edges = int_of_string num_edges_str in
                                let rec parse_edges acc lst =
                                        match lst with
                                        | dst_str :: weight_str :: tl ->
                                                        let dst = int_of_string dst_str in
                                                        let weight = float_of_string weight_str in
                                                        parse_edges ((dst, weight) :: acc) tl
                                        | [] -> List.rev acc
                                        | _ -> failwith "Corrupted edge information in graph.txt"
                                in
                                let edges = parse_edges [] rest in
                                if List.length edges <> num_edges then
                                        failwith (Printf.sprintf "Expected %d edges but found %d" num_edges (List.length edges));
                                adj_out.(i) <- edges
                | [] -> failwith "Empty line in graph file"
        done;
        close_in ic;
        adj_out
        (*
        let raw_out = Array.make n_nodes [] in
        let rec add_edge cnt =
                if cnt >= n_edges then ()
                else
                        let src = Random.int n_nodes in
                        let dst = Random.int n_nodes in
                        if src <> dst && not (List.mem dst raw_out.(src)) then          (* ensuring edge is not self-loop and is unique *)
                                begin
                                        raw_out.(src) <- dst :: raw_out.(src);
                                        add_edge (cnt + 1)
                                end
                        else
                                add_edge cnt
        in
        add_edge 0;
        let adj_out =
                Array.map (fun nbrs ->
                        let ws = List.map (fun _ -> Random.float 1.) nbrs in            (* generating random weights for every edge*)
                        let total_wt = List.fold_left ( +. ) 0.0 ws in                  (* total outgoing weight from node *)
                        List.map2 (fun nbr w -> (nbr, w /. total_wt)) nbrs ws           (* normalising the weights *)
                ) raw_out
        in
        adj_out
        *)


(* function to print the graph generated from the above function *)
let print_adj_list adj_out =
        Printf.printf "Adjacency List (Outgoing edge):\n";
        Array.iteri (fun i nbrs ->
                let s = nbrs
                        |> List.map (fun (j,edge_prob) -> Printf.sprintf "(%d, %.3f)" j edge_prob)
                        |> String.concat "; "
                in
                Printf.printf " Node %d -> [%s]\n" i s
        ) adj_out;
        Printf.printf "-------------- GRAPH PRINTED -----------------\n"


(* function to run the page rank algorithm *)
let rec loop rounds threshold actors=
        if rounds = 0 then Lwt.return_unit
        else
                let%lwt () = Lwt_list.iter_p send_state actors in
                let%lwt () = Lwt_list.iter_p receive_and_update actors in
                (*
                List.iter (fun actor ->
                        Printf.printf "In loop %d, Actor %d state: %.4f\n" (51 - rounds) actor.id actor.state
                ) actors;
                Printf.printf "----------------------------------\n";
                *)

                (* comparing the prev and curr states of actors to check for stability *)
                let count_non_converged_actors actors threshold = List.fold_left (fun acc actor ->
                        let diff = abs_float (actor.state -. actor.prev_state) in
                        actor.prev_state <- actor.state;
                        if diff <= threshold then acc else (acc + 1)
                ) 0 actors
                in
                let count = count_non_converged_actors actors threshold in
                if count = 0 then
                        begin
                                Printf.printf "Actors have converged\n";
                                Lwt.return_unit
                        end
                else 
                        loop (rounds - 1) threshold actors

(* Main entry loop *)
let () =
        Random.self_init ();
        let adj_out = read_graph () in
        let n_nodes = Array.length adj_out in
        (*print_adj_list adj_out;*)
       
        (* creating the streams for every node and initialising the actors *) 
        let streams_and_pushes = Array.init n_nodes (fun _ -> Lwt_stream.create ()) in
        let actors =
                Array.init n_nodes (fun i ->
                        let random_state = Random.float 10. in
                        {
                                id=i;
                                state=random_state;
                                prev_state = random_state;
                                send_tos=[];
                                recv_froms=[];
                        }
                )
        in

        (* assigning the channels to appropriate nodes *)
        Array.iteri (fun src edges ->
                List.iter (fun (dst, edge_prob) ->
                        let (stream, push_fn) = streams_and_pushes.(dst) in
                        actors.(src).send_tos <- (push_fn, edge_prob) :: actors.(src).send_tos;
                        actors.(dst).recv_froms <- stream :: actors.(dst).recv_froms
                ) edges
        ) adj_out;

        (* printing the initial states of the actors *)
        (*
        Array.iter (fun actor ->
                Printf.printf "Initial: Actor %d state = %.4f prev_state = %.4f\n" actor.id actor.state actor.prev_state
        ) actors;
        Printf.printf "================================\n";
        *)

        let actors_lst = Array.to_list actors in
        let threshold = 1.5 in
        Lwt_main.run (loop 52 threshold actors_lst)
