open util/ordering[State]

abstract sig Person {time: Int}
one sig A extends Person {} { time = 1 }
one sig B extends Person {} { time = 2 }
one sig C extends Person {} { time = 5 }
one sig D extends Person {} { time = 8 }

abstract sig Position {}
one sig Near extends Position {}
one sig Far extends Position {}

sig State {
    near: set Person,
    far: set Person,
    torch: Position,
    spent: Int -- time spent so far
}

sig Event {
    pre: State,
    post: State,
    toMove: set Person
} {
	-- between 1 and 2 people can cross the bridge
    some toMove
	#toMove < 3

	-- torch always changes sides
    (pre.torch = Near implies post.torch = Far) and
    (pre.torch = Far implies post.torch = Near)

    -- we don't lose any people along the way
    pre.torch = Near implies
		(toMove in pre.near and
		post.far = pre.far + toMove and
		post.near = pre.near - toMove)

    pre.torch = Far implies
            (toMove in pre.far and
            post.near = pre.near + toMove and
            post.far = pre.far - toMove)

	-- it takes the max amount of time to cross the bridge
	post.spent = plus[pre.spent, max[toMove.time]]
}

-- no time spent, torch is at the near end, along with all the people
fact initialState {
   	first.spent = 0
	first.torch = Near
	all a : Person |
		a in first.near
		no first.far
}

fact finalState {
	-- less than 15 minutes total crossing the bridge
	last.spent >= 0
	last.spent <= 15

	-- everyone is on the far side of the bridge
	all a : Person |
		a in last.far
		no last.near
}

fact trace {
	all s : State - last | 
		some e : Event |
			e.pre = s and e.post = s.next
}

run {} for 6 but 7 Int, 5 Event

/* One possible solution:

		(A, B) cross		2 min spent
		(B) comes back		4 min spent
		(C, D) cross		12 min spent
		(A) comes back		13 min spent
		(A, B) cross		15 min spent  (everyone has now crossed)

*/
