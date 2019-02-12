/* DO NOT EDIT ANYWHERE EXCEPT THE LINE "EDIT HERE" */

sig Node {}

/* copied from undirected graphs */
pred isUndirectedTree (graph: Node -> Node) {
  -- enforce symmetry
  ~graph in graph

  -- no self loops
  no iden & graph

  -- connected
  all disj u, v : Node | v in u.^graph

  -- only one path between two nodes; if there's a connection, remove it
  -- and see if there's still a path between nodes
  all disj u, v: Node |
        let c = (u -> v) |
          c in graph implies (v not in u.^(graph - (c + ~c)))
}

pred spans (graph1, graph2: Node -> Node) {
  -- make sure all edges in graph1 are also in graph2, 
  -- it covers all nodes, and then make it undirected
  all disj a, b : Node | 
    let e = (a -> b) |
      e in graph1 implies e in graph2
 
    let g1nodes = Node.graph1 + graph1.Node | 
      let g2nodes = Node.graph2 + graph2.Node |
        g1nodes = g2nodes

  isUndirectedTree[graph1]
}

pred showTwoSpanningTrees (graph, tree1, tree2: Node -> Node) {
  -- they both span graph
  spans[tree1, graph]
  spans[tree2, graph]

  -- trees are different
  some (tree1 - tree2) + (tree2 - tree1)
}

run showTwoSpanningTrees for 3
