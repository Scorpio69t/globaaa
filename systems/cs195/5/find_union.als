// State is just a point in time for our disjoint-set data structure
// We're only considering two states: pre-union and post-union
sig State {}

one sig StateA, StateB extends State {}

// Our disjoint-sets consists of nodes, which each have:
sig Node {
	parent: State -> one Node,       -- one parent (part of algorithm)
    root  : State -> one Node        -- one root (abstraction for modeling)
}

// Function for parents, makes binary operators easier (i.e. n.^(parents[s]))
fun parents[s: State]: Node -> Node {
	{n1, n2: Node | n1->s->n2 in parent}
}

// Mock-recursively (inductively) define root of each node in each state
fact defineRoot { all s: State, n: Node {
    -- any node that is its own parent is its own root
	let p = parents[s] | 
		(n = n.p implies n = s.(n.root)) and
    	-- otherwise, get its root from its non-self parent (recur)
		s.(n.root) = s.((n.p).root)
}}

// A union event joins two pre-state nodes in the post-state -- why don't we stipulate
// that n1 and n2 are disjoint
pred union { some disj n1, n2: Node | {
	let oldRoot = n2.root[StateA] |
		let newRoot = n1.root[StateA] | {
			-- set n1.root as parent of n2.root, no other parents altered
        	StateB.(oldRoot.parent) = newRoot 
			parents[StateB] - (oldRoot -> newRoot) = parents[StateA] - (oldRoot -> StateA.(oldRoot.parent))
      	}
}}

// See if the union operations look correct to you before formally checking
run unionexamples {union and all n: Node | one n.root[StateA]} for 5 Node, 2 State

// If our union operation is correct, find will preserved between pre and post
-- find is true of a state if that state is *well formed*
pred find[s: State] { all disj n1,n2: Node | {
	-- cycles should not exist in a clean find state
    (n1.parent[s] != n1 implies n1 not in n1.^(parents[s]))
    -- find expects all connected nodes to have the same root,
    -- and all disjoint nodes to have different roots
    sameRoot[n1,n2,s] iff connected[n1,n2,s]
}}

pred sameRoot[n1,n2: Node, s: State] {
	-- nodes n1 and n2 have the same root in state s
	s.(n1.root) = s.(n2.root)
}

pred connected[n1,n2: Node, s: State] {
	-- nodes n1 and n2 are connected if n1 can reach n2 and n2 can reach n1
	-- hint: use (with other things) the ~ of the parents function
	let nodesFromN1 = n1.^(parents[s]) |
		let nodesFromN2 = n2.^(parents[s]) | {
			-- inverse the path from a (node -> root) to a (root -> node)
			n2 in n1.^((parents[s] :> nodesFromN1) + (~(parents[s] :> nodesFromN2)))
			n1 in n2.^((parents[s] :> nodesFromN2) + (~(parents[s] :> nodesFromN1)))
	}
}
check unionPreservesFind { (find[StateA] and union) implies find[StateB] } for 5 Node, 2 State

pred isInitialState[s: State] {
	-- initial state if every node is its own parent
	all n : Node |
		s.(n.parent) = n
}

check initialStateWellFormed {
  all s: State |
    isInitialState[s] implies find[s]} for 5 Node, 2 State

pred buggyunion { some n1, n2: Node | {
    (n2.root[StateA]).parent[StateB] = n1.root[StateA]
    all n: Node - n2.root[StateA] - n1.root[StateA]  | n.parent[StateB] = n.parent[StateA]
}}

pred buggyunionfindWorks { (find[StateA] and buggyunion) implies find[StateB]}

run bufWorks {buggyunionfindWorks} for 7 Node, 2 State
check bufFails {buggyunionfindWorks} for 7 Node, 2 State

pred reason {
    // reason only applies when buggyunion is in effect,
    // and we have a good pre-state:
    buggyunion
    find[StateA]
    // Characterize the instances where buggy union fails
    ------------------------
    /* FILL */
    ------------------------ 

	-- the whole idea here is to notice that the parent nodes in StateB should be
	-- the same as in StateA except for possibly 1; additionally, looking at the
	-- states in bufWorks and bufFails, it seems to fail when StateB turns out to
	-- have a cycle in it so constrain against this as well 
	#(parents[StateB] - parents[StateA]) > 1 and
	some n : Node |
		n not in n.(parents[StateB]) and n in n.^(parents[StateB])
}
check bufFailsImpliesReason { (not buggyunionfindWorks) implies reason } for 7 Node, 2 State
check reasonImpliesBufFails { reason implies (not buggyunionfindWorks) } for 7 Node, 2 State
