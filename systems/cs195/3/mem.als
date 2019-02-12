-------------------------------------------------
--					Part A
-------------------------------------------------

-- we'll have different kinds of memory extend this
abstract sig Memory{}

-- interesting that we're saying there's only a single stack 
-- when in a computer there are numerous threads with their own
-- stacks
sig HeapCell extends Memory {}
one sig Stack extends Memory {}

-- maybe we can have multiple HeapCells pointing to eachother, 
-- but to allocate one it has to be off the Stack
abstract sig State { 
	allocated : set HeapCell,
	references : Memory -> HeapCell 
}

one sig StateA, StateB, StateC extends State {}

-- determine if m can be reached from the stack in state
pred reachFromStack[m : HeapCell, state : State] {
	-- find HeapCells connected to the stack
	let connectedToStack = Stack.(state.references) |
		-- m has to be in one of these or along transitive closure
		m in connectedToStack or
		m in connectedToStack.^(state.references)
}

-- see if all reachable memory is allocated; doesn't mean all allocated memory is reachable
pred safe[state : State] {
	let connectedToStack = Stack.(state.references) | 
		(connectedToStack + connectedToStack.^(state.references)) in state.allocated
}

-- see if all allocated memory is reachable
pred clean[state : State] {
	let connectedToStack = Stack.(state.references) | 
		state.allocated in (connectedToStack + connectedToStack.^(state.references)) 
}

-- I think the set of references can change, but not the allocated memory cells
fact A_to_B_AllocatedUnchanged {
	StateA.allocated = StateB.allocated
}

-- referecnes between B and C have to be the same & a memory cell in C won't be allocated
-- only if it's ref count in B = 0
fact B_to_C_GarbageCollected {
	StateB.references = StateC.references
	all c : Memory | c not in StateC.allocated iff ref_count[c, StateB] = 0
}

fun ref_count[m : HeapCell, state: State] : Int {
	#((state.references).m)
}


-- works now
check soundness { safe[StateA] implies safe[StateC] } for 4 Memory, 4 int

-- still fails after part A; I think this is for a number of reasons: self loops in the references.
-- no requirements that there are references between memory, etc
check completeness { clean[StateA] => clean[StateC] } for 4 Memory, 4 int

-- for testing
run show { } for exactly 4 Memory, 4 Int


-------------------------------------------------
--					Part B
-------------------------------------------------


fact additionalModelingConstraint {
	all s: State |
		all h: HeapCell |
			some s.references[h] implies h in s.allocated
}

pred incompleteness { clean[StateA] and not clean[StateC] }

run incompleteExamples { incompleteness } for 4 Memory, 4 int

run completeExamples { not incompleteness } for 4 Memory, 4 int

-- all allocated memory in StateA is reachable, and all allocated memory in StateC
-- is also reachable; true when incomplete
pred description { 
	--some s : StateB | 
	--	some m : HeapCell | 
			
			
}

check incompleteImpliesDescription {
	incompleteness implies description
} for 4 Memory, 4 int

--check descriptionImpliesIncomplete {
--	description and clean[StateA] implies incompleteness
--} for 4 Memory, 4 int
