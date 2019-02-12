/* DO NOT EDIT THE STENCIL FILE UNLESS WE ALLOW */

open util/ordering[State]

sig Vertex { }
sig State {
    L: seq Vertex, -- List of sorted elements
    S: set Vertex, -- Set of nodes with no incoming edges
    Graph: Vertex -> Vertex
}{
	-- Graph is acyclic
    all v : Vertex | 
		v not in v.^Graph
}

abstract sig Event {
    pre, post: State,
    v: Vertex -- the chosen vertex for an iteration
}

fact initialState {
	-- begins with empty L
	first.L.isEmpty and
	-- all nodes with no incoming edges
	first.S = Vertex - Vertex.(first.Graph) and
	-- first graph has all vertices
	all v : Vertex |
		some v.(first.Graph) or some (first.Graph).v
}

fact trace {
	-- order states
	all s : State - last | 
		some e : Event | 
			e.pre = s and e.post = s.next
}

-- change S 
pred BuggyChangeS[v : Vertex, present : State, post : State] {
	-- find all nodes v relates to
	let m = v.(present.Graph) | 
		post.S = present.S - v + m
}

-- add to the tail of L
pred AddToL[v : Vertex, present : State, post : State] {
	-- find size of L
	let size = #(present.L) |
		-- this is what seq would be with v added onto tail
		let added = present.L.insert[size, v] |	
			-- the vertex you're adding shouldn't be in L	
			v not in present.L.elems and
			-- but it should be in the next state's L
			post.L = added
}

-- remove edges from post.Graph
pred UpdateGraph[v : Vertex, present : State, post : State] {
-- does it make any difference to have v->(v.(pre.Graph)) instead of v->Vertex?
	post.Graph = present.Graph - (v -> Vertex)
}

-- change S correctly
pred CorrectChangeS[v : Vertex, present : State, post : State] {
	-- find all nodes v relates to
	let m = v.(present.Graph) | 
		let withoutIncoming = m - Vertex.((present.Graph) - (v -> m)) | 
			post.S = present.S - v + withoutIncoming
}

sig IterationEventBuggy extends Event {
}{
	some v : pre.S |
		BuggyChangeS[v, pre, post] and
		AddToL[v, pre, post] and
		UpdateGraph[v, pre, post]	
}

sig IterationEventCorrect extends Event {
}{
	some v : pre.S |
		CorrectChangeS[v, pre, post] and
		AddToL[v, pre, post] and
		UpdateGraph[v, pre, post]	
}

assert correctness {
	-- if our original graph was empty, L should be empty too
	#(first.Graph) = 0 and last.L.isEmpty or

	all disj v, w : Vertex |
		let i = last.L.idxOf[v] |
			let j = last.L.idxOf[w] |
				i < j implies v not in w.^(first.Graph)

	-- Nicole's
--	Vertex in univ.(last.L)
--	!last.L.hasDups

--	all disj v1, v2: Vertex | (last.L.idxOf[v1] > last.L.idxOf[v2]) implies (v1->v2 not in first.Graph)

}

-- Ina’s algorithm:
check correctness for 8 but 7 Vertex, 0 IterationEventCorrect, 8 Int -- This should have a counterexample
run {} for 8 but 7 Vertex, 0 IterationEventCorrect, 8 Int -- This should have an instance (though might be incorrect)

-- Correct algorithm:
check correctness for 8 but 7 Vertex, 0 IterationEventBuggy, 8 Int -- This should have no counterexample
run {} for 8 but 7 Vertex, 0 IterationEventBuggy, 8 Int -- This should have a correct instance
