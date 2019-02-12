open Ast ;;

let rec bfs (t : tree list) : unit =
  match t with
  | [] -> ()
  | hd :: tl -> (match hd with
                 | Leaf -> ()
                 | Tree(l, i, r) -> print_int i; print_string " "; bfs (tl @ [l; r]))
;;

let t = Tree(Leaf, 10, Leaf) ;;
let t' = Tree(t, 11, Tree(Leaf, 12, Leaf)) ;;
let t'' = Tree(t', 13, t) ;;

(* should print out '10'; '11 10 12'; '13 11 10 10 12' *)
let _ = bfs [t]; print_string "\n"; bfs [t']; print_string "\n"; bfs [t'']; print_string "\n" ;;

