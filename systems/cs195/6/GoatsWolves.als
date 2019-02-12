open util/ordering[State]

// Characters
abstract sig Character {}
one sig Boat extends Character {}
sig Goat extends Character {}
sig Wolf extends Character {}
fact { 
	#Goat = #Wolf -- goat:wolf ratio is 1:1
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
pred noEating[s: one State, side: one Side] {
	-- true if there are no goats on this side or
	(no s.sideof[Goat] & side) or 
	-- *if there are more goats than wolves
	(#{g: Goat | s.sideof[g] = side} >= #{w: Wolf | s.sideof[w] = side})	
}

pred solvePuzzle {
	-- a state ordering is a solution to the puzzle if no goat ever gets eaten
	all s : State | noEating[s, Near] and noEating[s, Far]
}
run solvePuzzle for exactly 3 Goat, exactly 3 Wolf, 12 State

