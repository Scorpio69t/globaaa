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
abstract sig State {
    sideof : Character -> one Side,
    next: lone State
}
one sig StateA, StateB, StateC extends State {}
pred crosses[c: Character, s: State] {
    -- character is on the boat side
    s.sideof[c] = s.sideof[Boat]
    -- they cross to the other side with the boat
    s.next.sideof[c] = Side-s.sideof[Boat]
}
pred stays[c: Character, s: State] {
    -- character stays put
    s.next.sideof[c] = s.sideof[c]
}
fact {
    -- state constraints
    StateA.next = StateB
    StateB.next = StateC
    -- transition constraints
    all s: State - StateC | {
            crosses[Boat, s] -- boat crosses every state
            all c:Character-Boat | crosses[c,s] or stays[c, s] -- characters follow rules
            //removed for study
            //some c:Character-Boat | crosses[c,s] -- boat cannot be empty
    }
}

pred progress {
    #{c : Character | StateA.sideof[c] = Far} < #{c : Character | StateC.sideof[c] = Far}
}
pred noEating[s: one State, side: one Side] {
    -- true if there are no goats on this side or
    (no s.sideof[Goat] & side) or
    -- *if there are more goats than wolves
    (#{g: Goat | s.sideof[g] = side} >= #{w: Wolf | s.sideof[w] = side})    
}

pred preservation { all side: Side | {
    noEating[StateA, side]
    noEating[StateB, side]
    noEating[StateC, side]
}}

fact assuming { preservation }
run progress {progress} for 5 Goat, 5 Wolf, 5 Int
run noProgress {not progress} for 5 Goat, 5 Wolf, 5 Int

pred reason { some nearToFar: StateA+StateB | some farToNear: StateA+StateB | {
    nearToFar.sideof[Boat] = Near
    farToNear.sideof[Boat] = Far

	-- nothing was in the boat on the initial trip from Near to Far
	-- #(StateB.(sideof.Far)) = #(StateA.(sideof.Far))

	-- if boat is on near side, the initial trip to far side carries less characters than return trip	
	let initialTrip = minus[#(StateB.(sideof.Far) - Boat), #(StateA.(sideof.Far) - Boat)] | {
		let returnTrip = minus[#(StateB.(sideof.Far) - Boat), #(StateC.(sideof.Far) - Boat)] | {
			Boat in StateA.(sideof.Near)
			returnTrip >= initialTrip
		}
	} 

	or

	-- if boat is on far side, initial trip carres more characters than return trip
	let initialTrip = minus[#(StateB.(sideof.Near) - Boat), #(StateA.(sideof.Near) - Boat)] | {
		let returnTrip = minus[#(StateB.(sideof.Near) - Boat), #(StateC.(sideof.Near) - Boat)] | {
			Boat in StateA.(sideof.Far)
			returnTrip <= initialTrip
		}
	}
}}

check reasonImpliesNoProgress {reason implies (not progress)} for 5 Goat, 5 Wolf, 5 Int
check noProgressImpliesReason {(not progress) implies reason} for 5 Goat, 5 Wolf, 5 Int
