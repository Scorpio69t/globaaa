------------------------------- MODULE light -------------------------------
VARIABLES color1, color2, turn

vars == <<color1,color2,turn>>

(* TLA+ is untyped, so we have a type-check formula separately *)
TypeOK == /\ (color1 = "red" \/ color1 = "green" \/ color1 = "yellow") 
          /\ (color2 = "red" \/ color2 = "green" \/ color2 = "yellow")
          /\ (turn = 1 \/ turn = 2)                    
          
(* Specify starting state. Note this isn't really /asserted/ until we get to Spec, which is what is "run" *)
Init == /\ color1 = "red"
        /\ color2 = "red"
        /\ turn = 1

(* Specify valid transitions. Again, not really asserted until we get to Spec *)


Move1 == /\ turn = 1
         /\ UNCHANGED color2 
         /\ CASE color1 = "red"    -> color1' = "green"  /\ UNCHANGED turn
              [] color1 = "green"  -> color1' = "yellow" /\ UNCHANGED turn
              [] color1 = "yellow" -> color1' = "red"    /\ turn' = 2

Move2 == /\ turn = 2
         /\ UNCHANGED color1 
         /\ CASE color2 = "red"    -> color2' = "green"  /\ UNCHANGED turn
              [] color2 = "green"  -> color2' = "yellow" /\ UNCHANGED turn
              [] color2 = "yellow" -> color2' = "red"    /\ turn' = 2

Next == Move1 \/ Move2

(* Make ABSOLUTELY SURE you're checking as "temporal formula" not init/next. Because the /spec/ is what adds fairness. *)

Spec == /\ Init
        (* G of an Action operator: Globally true that Action happens or vars' = vars (stutter)*) 
        /\ [][Next]_vars
        (* Temporal operator: Weak Fairness for Next: whenever continuously enabled, eventually taken *)
        /\ WF_vars(Next)   

AlwaysSomeRed == [] (color1 = "red" \/ color2 = "red") 
AlwaysEventuallyGreen == /\ []<>(color1 = "green")
                         /\ []<>(color2 = "green")

=============================================================================
