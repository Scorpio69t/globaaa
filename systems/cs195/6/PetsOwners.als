open util/ordering[State]

// Characters
abstract sig Character {}
one sig Boat extends Character {}
sig Pet extends Character { owner : one Owner }
sig Owner extends Character { pet : one Pet }
fact {
	#Pet = #Owner								-- pet:owner ratio is 1:1
	no disj p1, p2 : Pet | p1.owner = p2.owner 	-- *no pets have the same owner
	no disj o1, o2 : Owner | o1.pet = o2.pet 	-- *no owners have the same pet
	all o : Owner | o.pet.owner = o 			-- *an owner's pet's owner is the owner
	all p : Pet | p.owner.pet = p 				-- *a pet's owner's pet is the pet
}

// State
abstract sig Side {}
one sig Near, Far extends Side {}
sig State { sideof : Character -> one Side }
pred cross[s: State] { 
	some disj a, b : Character {
		-- pick two distinct boat side characters
		s.sideof[a] = s.sideof[Boat]
		s.sideof[b] = s.sideof[Boat]
		-- move them and the boat to the other side
		s.next.sideof[a] = Side-s.sideof[Boat]
		s.next.sideof[b] = Side-s.sideof[Boat]
		s.next.sideof[Boat] = Side-s.sideof[Boat]
		-- everyone else stays put
		all c:Character-a-b-Boat | s.next.sideof[c] = s.sideof[c]
	}
}
fact {
	all c:Character | first.sideof[c] = Near	-- all on the near side in the first state
	all c:Character | last.sideof[c] = Far 		-- all on the far side in the last state
	all s: State - last | cross[s]				-- one cross event per state
}

// Solution
pred noJealousy[s: one State, side: one Side] {
	-- true if there are no owners on this side or
	(no s.sideof[Owner] & side) or 
	-- *if all the pets' owners are present
	(all p : Pet | s.sideof[p] = side implies s.sideof[p.owner] = side)	
}
pred solvePuzzle {
	-- a state ordering is a solution to the puzzle if no pet ever gets jealous
	all s : State | noJealousy[s, Near] and noJealousy[s, Far]
}
run solvePuzzle for exactly 3 Pet, exactly 3 Owner, 12 State
