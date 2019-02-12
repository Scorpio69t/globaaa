/* DO NOT EDIT ANYWHERE EXCEPT THE LINE "EDIT HERE" */

sig Node {
  succ: set Node
}

pred isRing {
  -- every ring has to have a relation to and from it
  all r : Node | some r.succ and some succ.r

  -- only one relation per node
  succ.~succ in iden

  -- everything is reachable
  all a, b : Node | b in a.^succ
}

run isRing for 4 Node
