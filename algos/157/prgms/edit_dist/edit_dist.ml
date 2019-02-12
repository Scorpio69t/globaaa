exception Hderror of string ;;

(* helper functions *)
let head (lst : 'a list) : 'a = 
  match lst with
  | [] -> raise(Hderror "empty list")
  | hd :: _ -> hd
;;

let tail (lst : 'a list) : 'a list = 
  match lst with
  | [] -> []
  | _ :: tl -> tl
;;

let b_to_i (b : bool) : int = if b then 0 else 1 ;;

(* find the minimum value in a triple *)
let min (triple : int * int * int) : int = 
  match triple with
  | (a, b, c) -> (if a <= b && a <= c then a
                  else (if b <= a && b <= c then b
		        else c))
;;

assert(min(1, 2, 3) == 1) ;;
assert(min(2, 1, 3) == 1) ;;
assert(min(2, 3, 1) == 1) ;;
assert(min(1, 1, 2) == 1) ;;

(* for ease of use, let's make w1 and w2 lists of chars *)
let rec edit_dist (w1 : char list) (w2 : char list) (i : int) (j : int) : int =
  if i == 0 then 0
  else (if j == 0 then 0
        else min(1 + edit_dist w1 (tail w2) i (j - 1),
                 1 + edit_dist (tail w1) w2 (i - 1) j,
                 b_to_i ((head w1) == (head w2)) + edit_dist (tail w1) (tail w2) (i - 1) (j - 1)))
;;

(* tests *)
let _ = print_int (edit_dist ['m'; 'a'; 'k'; 'e'] ['m'; 'o'; 'd'; 'e'] 4 4); print_string "\n" ;;
let _ = print_int (edit_dist ['d'; 'r'; 'a'; 'g'; 'o'; 'n'] ['d'; 'r'; 'a'; 'u'; 'g'; 'h'; 't'] 6 7); print_string "\n" ;;
let _ = print_int (edit_dist ['s'; 'u'; 'n'; 'n'; 'y'] ['s'; 'n'; 'o'; 'w'; 'y'] 5 5); print_string "\n" ;;
let _ = print_int (edit_dist ['e'; 'x'; 'p'; 'o'; 'n'; 'e'; 'n'; 't'; 'i'; 'a'; 'l'] ['p'; 'o'; 'l'; 'y'; 'n'; 'o'; 'm'; 'i'; 'a'; 'l'] 11 10); print_string "\n" ;;
