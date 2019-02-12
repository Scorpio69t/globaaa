/* DO NOT EDIT ANYWHERE EXCEPT THE LINE "EDIT HERE" */

sig Node {
  children: set Node
}

pred isDirectedTree {
  -- children only have one parent
  children.~children in iden 

  -- no cycles
  no iden & children.^children

  -- Node.children returns a set of all nodes in the codomain
  -- of relations; there can only be one or none (i.e. the root)
  -- that are never in the codomain
  lone (Node - Node.children)
}

run isDirectedTree for 4 Node

/* 
   Just as a note to myself: super useful to look at this
   in the evaluator. If we print out Node, it's a set of
   all Nodes while Node.children are all Nodes in the
   codomain of a relation.

   I was slightly unclear when starting whether using the 
   sig Node in relational logic would look at the Nodes
   one at a time or consider them as a set.
*/
