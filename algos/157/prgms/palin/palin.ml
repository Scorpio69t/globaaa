let max ((v1, v2, v3) : int * int * int) : int =
  (if v1 >= v2 && v1 >= v3 then v1
   else (if v2 >= v1 && v2 >= v3 then v2
         else v3))
;;

let rec palin (l : char list) (l' : char list) (acc : int) (lpos : int) (rpos : int) : int = 
  if lpos == rpos then acc + 1
  else (if rpos - lpos == 1 then acc + 2
  else (if lpos > rpos then acc
	else match l with
  	     | [] -> acc
	     | hd :: tl -> (match l' with
			    | [] -> acc
			    | hd' :: tl' -> max((if hd == hd' then palin tl tl' (acc + 2) (lpos + 1) (rpos - 1)
					     	 	      else palin tl tl' 0 (lpos + 1) (rpos - 1)), 
					  	       		   palin l tl' 0 (lpos) (rpos - 1), 
							           palin tl l' 0 (lpos + 1) (rpos)))))
;;

assert (palin ['h'; 'i'] ['i'; 'h'] 0 1 2 == 2);
assert (palin ['h'; 'e'; 'l'; 'l'; 'o'] ['o'; 'l'; 'l'; 'e'; 'h'] 0 1 5 == 2);
assert (palin ['r'; 'a'; 'c'; 'e'; 'c'; 'a'; 'r'] ['r'; 'a'; 'c'; 'e'; 'c'; 'a'; 'r'] 0 1 7 == 7);
assert (palin ['e'; 'l'; 'l'; 'o'; 'h'; 'h'; 'o'] ['o'; 'h'; 'h'; 'o'; 'l'; 'l'; 'e'] 0 1 7 == 4);
assert (palin ['m'; 'o'; 'm'; 'i'; 's'; 'm'; 's'; 'i'; 'h'] ['h'; 'i'; 's'; 'm'; 's'; 'i'; 'm'; 'o'; 'm'] 0 1 9 == 5);
