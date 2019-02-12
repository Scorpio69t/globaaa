/* DO NOT EDIT ANYWHERE EXCEPT THE LINE "EDIT HERE" */

sig Node {
  edges: set Node
}

pred isUndirectedTree {
  -- enforce symmetry
  ~edges in edges

  -- no self loops
  no iden & edges

  -- connected
  all disj u, v : Node | v in u.^edges

  -- only one path between two nodes; if there's a connection, remove it
  -- and see if there's still a path between nodes
  all disj u, v: Node | 
	let c = (u -> v) |
	  c in edges implies (v not in u.^(edges - (c + ~c)))
}

run isUndirectedTree for 4 Node
 
/*
   Note to self: in the last condition, you can't replace 'implies' with
   'and'. 'and' would mean that for every disj. set of nodes u, v, there is
   a relation between them in edges. This would introduce cycles for anything
   greater than 2 nodes. 'implies', on the other hand, says that *if it's the
   case* that there is a (u -> v) in edges, then removing it should eliminate
   all paths between u and v unless there's a cycle.
*/
