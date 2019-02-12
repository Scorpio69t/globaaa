open Ast ;;

let rec dfs (t: tree) : unit =
  match t with
  | Leaf -> ()
  | Tree(l, i, r) -> dfs l; dfs r; print_int i; print_string " "
;;

let t = Tree(Leaf, 10, Leaf) ;;
let t' = Tree(t, 11, Tree(Leaf, 12, Leaf)) ;;

(* should print out '10'; '10 12 11' *)
let _ = dfs t; print_string "\n"; dfs t'; print_string "\n" ;;
