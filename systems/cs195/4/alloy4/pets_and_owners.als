open util/ordering[State]

abstract sig Animal {}
sig Pet extends Animal {
    owned: Owner
}
sig Owner extends Animal {}

-- each pet has only a single owner
fact ownedMakesSense {
	all p : Pet |
		some o : Owner |
			o in p.owned and o not in (Pet - p).owned
}

abstract sig Position {}
one sig Near extends Position {}
one sig Far extends Position {}

sig State {
    near: set Animal,
    far: set Animal,
    boat: Position
} {
    noStealing[near]
    noStealing[far]
}

-- if one owner is there, all the pets' owners must also be there
pred noStealing[animals: set Animal] {
	all o : Owner |
		o in animals implies animals.owned in animals 
}

sig Event {
    pre: State,
    post: State,
    toMove: set Animal
} {
	some toMove
	#(toMove) < 3

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

run {} for 12 but exactly 11 Event, exactly 3 Pet, exactly 3 Owner, 4 Int

/*

	Owner0 -> Pet2
	Owner1 -> Pet0
	Owner2 -> Pet1

	possible solution:

		(Owner0, Pet2)		cross
		(Owner0)			comes back
		(Pet1, Pet3) 		cross
		(Pet3)				comes back
		(Owner0, Owner2)	cross
		(Owner2, Pet1)		comes back
		(Owner1, Owner2)	cross
		(Pet2)				comes back
		(Pet0, Pet2)		cross
		(Pet2)				comes back
		(Pet1, Pet2)		cross			(everyone has crossed)
		
*/
