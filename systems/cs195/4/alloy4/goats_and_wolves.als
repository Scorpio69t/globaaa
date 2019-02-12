open util/ordering[State]

abstract sig Animal {}
sig Goat extends Animal {}
sig Wolf extends Animal {}

abstract sig Position {}
one sig Near extends Position {}
one sig Far extends Position {}

sig State {
    near: set Animal,
    far: set Animal,
    boat: Position
} {
    noEating[near] -- for any state there’s no eating
    noEating[far]
}

-- on any side, there has to be more goats than wolves unless there
-- are no goats at all
pred noEating [animals: set Animal] {
	let g = Goat & animals | 
		let w = Wolf & animals |
			#g >= #w or #g = 0
}

sig Event {
    pre: State,
    post: State,
    toMove: set Animal
} {
	-- moving something
	some toMove 
	#toMove < 3

	-- boat always changes sides
	(pre.boat = Near implies post.boat = Far) and
	(pre.boat = Far implies post.boat = Near) 

	-- we don't lose any animals along the way
	pre.boat = Near implies
			(toMove in pre.near and
			post.far = pre.far + toMove and
			post.near = pre.near - toMove)

	pre.boat = Far implies 
		(toMove in pre.far and
		post.near = pre.near + toMove and
		post.far = pre.far - toMove)
}

fact initialState {
	all a : Animal |
		a in first.near and no first.far and first.boat = Near
}

fact finalState {
	all a : Animal |
		a in last.far and no last.near
}

fact trace {
	all s : State - last |
		some e : Event |
			e.pre = s and e.post = s.next
}

run {} for 12 but exactly 11 Event, exactly 3 Goat, exactly 3 Wolf, 4 Int

/*

	one possible solution:

		(Goat1, Wolf0) 		cross	
		(Goat1)				comes back
		(Wolf1, Wolf2)		cross
		(Wolf1)				comes back
		(Goat0, Goat2)		cross
		(Goat2, Wolf2)		come back
		(Goat1, Goat2)		cross
		(Wolf0)				comes back
		(Wolf0, Wolf2)		cross
		(Wolf2)				comes back
		(Wolf1, Wofl2)		cross		(all wolves have now crossed)
		
*/
