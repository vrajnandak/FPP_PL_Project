(* function to generate a random graph with 'n_nodes' nodes and 'n_edges' edges along with edge weights *)
let generate_random_graph n_nodes n_edges =
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
        let oc = open_out "graph.txt" in
        Printf.fprintf oc "%d\n" (Array.length adj_out);

        Array.iter (fun neighbors ->
                let num_edges = List.length neighbors in
                Printf.fprintf oc "%d" num_edges;
                List.iter (fun (dst, edge_prob) ->
                        Printf.fprintf oc " %d %.6f" dst edge_prob 
                ) neighbors;
                output_char oc '\n'
        ) adj_out;
        close_out oc;
        ()

let () =
        let nodes = int_of_string Sys.argv.(1) in
        let edges = int_of_string Sys.argv.(2) in
        generate_random_graph nodes edges
