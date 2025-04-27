open Lwt.Infix

type actor = {
        id : int;
        mutable state : float;
        mutable send_tos : (float option -> unit) list;
        mutable recv_froms : float Lwt_stream.t list;
}

let send_state actor =
        List.iter (fun push ->
                Printf.printf "Actor %d is sending value: %f\n" actor.id actor.state;
                push (Some actor.state)
        ) actor.send_tos;
        Lwt.return_unit

let receive_and_update actor =
        let receive_all = List.map (fun stream -> Lwt_stream.next stream) actor.recv_froms in
        Lwt.all receive_all >|= fun received_values ->
        let total_received = List.fold_left ( +. ) 0.0 received_values in
        Printf.printf "Actor %d received total: %f\n" actor.id total_received;
        actor.state <- actor.state +. total_received

(* still have to restraint the graph to make it a markov chain to see stabilized output *)
let generate_random_graph n_nodes n_edges =
        let adj_out = Array.make n_nodes [] in
        let adj_in = Array.make n_nodes [] in
        let rec add_edge count =
        if count >= n_edges then ()
        else
                let from_node = Random.int n_nodes in
                let to_node = Random.int n_nodes in
                if from_node <> to_node && not (List.mem to_node adj_out.(from_node)) then begin
                adj_out.(from_node) <- to_node :: adj_out.(from_node);
                adj_in.(to_node) <- from_node :: adj_in.(to_node);
                add_edge (count + 1)
                end else
                add_edge count
        in
        add_edge 0;
        (adj_out, adj_in)

let print_adj_list adj_out adj_in =
        Printf.printf "Adjacency List (Outgoing edges):\n";
        Array.iteri (fun i neighbors ->
                Printf.printf "Node %d -> [%s]\n" i (String.concat ", " (List.map string_of_int neighbors))
        ) adj_out;
        Printf.printf "\nAdjacency List (Incoming edges):\n";
        Array.iteri (fun i neighbors ->
                Printf.printf "Node %d -> [%s]\n" i (String.concat ", " (List.map string_of_int neighbors))
        ) adj_in;
        print_newline ()

let print_actors_states loop_number actors =
        Array.iter (fun actor ->
                Printf.printf "After loop %d: Actor %d state = %f\n" loop_number actor.id actor.state
        ) actors;
        print_endline "---------------------------------"


let rec loop n actors =
        if n = 0 then Lwt.return_unit
        else
                let%lwt () = Lwt_list.iter_p send_state (Array.to_list actors) in
                let%lwt () = Lwt_list.iter_p receive_and_update (Array.to_list actors) in
                let%lwt () =
                Lwt_list.iter_p (fun actor ->
                        Lwt_io.printf "Actor %d updated state: %f\n" actor.id actor.state
                ) (Array.to_list actors)
                in
                Lwt_io.printl "==== End of iteration ====" >>= fun () ->
                        loop (n - 1) actors


let () =
        Random.self_init ();
        let n_nodes = 4 in
        let n_edges = 5 in
        let adj_out, adj_in = generate_random_graph n_nodes n_edges in

        print_adj_list adj_out adj_in;

        let actors = Array.init n_nodes (fun id ->
                {
                id = id + 1;
                state = float_of_int (Random.int 10);
                send_tos = [];
                recv_froms = [];
                }
        ) in

        (* creating one stream for every node since we're currently sending messages and only then moving to receiving messages *)
        let streams_and_pushes = Array.init n_nodes (fun _ ->
                Lwt_stream.create ()
        ) in
  
        (* connecting the streams to actors based on connectivity of graph *)
        Array.iteri (fun from_id outgoing_neighbors ->
                List.iter (fun to_id ->
                        let (stream, push_fn) = streams_and_pushes.(to_id) in
                        actors.(from_id).send_tos <- push_fn :: actors.(from_id).send_tos;
                        actors.(to_id).recv_froms <- stream :: actors.(to_id).recv_froms;
                ) outgoing_neighbors
        ) adj_out;

        print_actors_states 0 actors;

        Lwt_main.run (loop 5 actors)
