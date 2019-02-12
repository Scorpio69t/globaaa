open util/ordering[State]

sig Element {}

sig State {
    elements: seq Element
}

abstract sig Event {
    pre, post: State
}

fact initialState {
    -- initial state is that the stack is empty
	first.elements.isEmpty
}

fact trace {
	all s : State - last |
		some e : Event |
			e.pre = s and e.post = s.next
}

sig Push extends Event {
    value: Element
}{
	-- insert element at end of stack
	let size = #(pre.elements) |
		post.elements = pre.elements.insert[size, Element]
}

sig Pop extends Event {
}{
	-- take element from end of stack if size > 0
	let size = #(pre.elements) |
		size > 0 and post.elements = pre.elements.delete[minus[size, 1]]
}

-- HOMEWORK ASSERTION 1
assert popThenPush {
	-- why does this have to be all instead of some?
	all e : Pop |
		some e' : Push |
			-- pop then push are sequential and the last element the same
			(e.post = e'.pre and
			e.pre.elements.last = e'.post.elements.last)
			-- implies nothing else has changed
			implies e.pre.elements = e'.post.elements
}
check popThenPush for 5

-- HOMEWORK ASSERTION 2
assert sameNumberPushesPops {
	#(Pop) = #(Push) implies last.elements.isEmpty
}
check sameNumberPushesPops for 5 but 6 Int

-- HOMEWORK ASSERTION 3
assert noPopFromEmpty {
    all e : Pop |
		#(e.pre.elements) != 0
}
check noPopFromEmpty for 5
