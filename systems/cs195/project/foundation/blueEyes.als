open util/ordering[Day]

abstract sig Agent {}
abstract sig NonBlue extends Agent {}

sig Blue extends Agent {}
sig Brown extends NonBlue {}
sig Green extends NonBlue {}


/* Kripke structure for worlds */
sig World {
	-- each world has a set of agents that believe they're 
	-- either blue or not blue
	blues: set Agent,
	nonblues: set Agent,
	browns: set Agent,
	greens: set Agent
}{
	all a : Agent | {
		-- agent has to be in a world
		a in blues iff a not in nonblues
	}

	browns in nonblues and greens in nonblues

	all nb : nonblues | {
		nb in browns iff nb not in greens
	}
}

/* Temporal ordering */
sig Day {
	Nth: Int,

	-- those that know their eye color will leave
	left: set Agent,

	-- these are the worlds in a particular day; the relations will be 
	-- captured in worlds.beliefs
	worlds: set World,

	-- from each world is a set or relations to other worlds
	-- believed to be possible by a particular agent
	beliefs: set (World -> (Agent -> World))
} 

/* Used to transition from Day to Day */
sig Event {
	pre: Day,
	post: Day
} {
	-- increment counter as we go
	post.Nth = add[pre.Nth, 1]

	-- as we parse the Kripke structure, we won't add worlds or
	-- relations that weren't there on a previous day i.e. transitions
	-- only are destructive in terms of possibilities
	post.worlds in pre.worlds
	post.beliefs in pre.beliefs

	-- someone leaves when they know their eye color in the actual world
	all a : Agent |
		someoneKnows[pre, a] iff a in post.left 

	-- as we transition, update deleted worlds based on agents' beliefs
	all w : pre.worlds | {
		transitionChange[pre, w] iff w not in post.worlds
	} 

	-- delete the relations going to/from all deleted worlds
	let deleted = pre.worlds - post.worlds | {
		post.beliefs = pre.beliefs - (World->(Agent->deleted) + deleted->(Agent->World))
	}
}

/* Relate all States and Events */
fact trace {
	all s: Day - last | {
		some e: Event | 
			e.pre = s and e.post = s.next
	} 
}

/* Set up initial belief system */
fact init {
	first.Nth = 0
	no first.left			-- no one knows their eye color initially
	initialBeliefs
	complete
	World in first.worlds		-- all worlds are possible first day
}

/* Each agent believes in their current world and any world that
 * deviates by only their eye color */
pred initialBeliefs {
	all disj w1, w2 : first.worlds |
		all a : Agent | {
			-- self loop; each agent believes in their world possible
			(w1->a->w1) in first.beliefs

			-- each agent believes in a world where only their eye color is different
			let blueToNonBlue = (a in w1.blues and w2.nonblues = (w1.nonblues + a) and w2.blues = (w1.blues - a)) |
			let nonBlueToBlue = (a in w1.nonblues and w2.blues = (w1.blues + a) and w2.nonblues = (w1.nonblues - a)) |
			let greenToBrown = (a in w1.greens and w2.browns = (w1.browns + a) and w2.greens = (w1.greens - a)) |
			let brownToGreen = (a in w1.browns and w2.greens = (w1.greens + a) and w2.browns = (w1.browns - a)) |

			(blueToNonBlue or nonBlueToBlue or greenToBrown or brownToGreen) iff (w1->a->w2) in first.beliefs
		}
}

/* All agents should be in a world & no two worlds 
 * should have the same number of blue/nonblue people */
pred complete {
	-- worlds are different
	all disj w1, w2 : World | {
		not (w1.blues = w2.blues and w1.greens = w2.greens and w1.browns = w2.browns)
	}
}

/* If the Guru speaks or someone in a world is misinformed,
 * update possible worlds/beliefs */
pred transitionChange[pre : Day, w : World] {
	guruSpeaks[pre, w] or misinformed[pre, w] or knowersLeft[pre, w]
}

/* Condition after the Guru speaks */
pred guruSpeaks[pre : Day, w : World] {
	pre = first and #(w.blues) = 0
}

/* There's some world connected to w1 and in this world someone
 * incorrectly holds some belief */
pred misinformed[pre : Day, w1 : World] {
	some w2 : pre.worlds | 
		some a : Agent |
			pre != first and w1 in Agent.(w2.(pre.beliefs)) and iKnow[w1, pre.beliefs, a] and not actualWorld[a, w1]
}

/* Must get rid of worlds where knowers would be uncertain */
pred knowersLeft[pre : Day, w : World] {
	some a: pre.left |
		not iKnow[w, pre.beliefs, a]
}

/* Know your eye color if you can only see into other worlds that 
 * confirm your beliefs AND current world conforms to actual world */
pred someoneKnows[pre : Day, a : Agent] {
	some w : pre.worlds |
		iKnow[w, pre.beliefs, a] and actualWorld[a, w]
}

/* I can only see into other worlds that confirm my beliefs */
pred iKnow[w : World, b : (World -> (Agent -> World)), a : Agent] {
	let worldsFromW = a.(w.b) | {
		all w' : worldsFromW | {
			-- only see worlds that confirm belief
			(a in w.blues and a in w'.blues) or (a in w.greens and a in w'.greens) or (a in w.browns and a in w'.browns)
		}
	}
}

/* Actual world (from a's perspective) */
pred actualWorld[a : Agent, w : World] {
	(a in w.blues and (#Green + #Brown) = (#(w.greens) + #(w.browns)) and (Blue - a) = (w.blues - a)) or
	(a in w.greens and Blue = w.blues and Brown = w.browns and (Green - a) = (w.greens - a)) or
	(a in w.browns and Blue = w.blues and Green = w.greens and (Brown - a) = (w.browns - a))
}

/* Check: first day is symmetric */
assert symmetricStart {
	all disj w1, w2 : first.worlds |
		all a : Agent |
			(w1->a->w2) in first.beliefs implies (w2->a->w1) in first.beliefs
}
check symmetricStart for exactly 2 Blue, exactly 0 NonBlue, exactly 4 World, exactly 2 Day, exactly 2 Event

/* Check: first day no one should know their eye color */
assert noOneKnows {
	all a : Agent |
		all w : first.worlds |
			not	iKnow[w,first.beliefs, a]
}
check noOneKnows for exactly 2 Blue, exactly 0 NonBlue, exactly 9 World, exactly 2 Day, exactly 2 Event

/* Blue eyed agents know their eye color on the nth day */
assert NthDay {
	all d: Day | d.Nth = add[#Blue, 1] implies Blue in d.left
}

/* Similar predicate to pass to correspondence */
pred correspondenceNthDay {
	all d: Day | d.Nth = add[#Blue, 1] implies Blue in d.left
}

/* Brown eyes should never know their eye color */
assert BrownStays {
	-- there's no day where nonblue is in left
	all d: Day | all nb: NonBlue | nb not in d.left
}

/* Similar predicate to pass to correspondence */
pred correspondenceBrownStays {
	-- there's no day where nonblue is in left
	all d: Day | all nb: NonBlue | nb not in d.left
}

/* As a note: # of worlds = (#colors)^(#Agent) = 3^2 = 9 */
check NthDay for exactly 1 Blue, exactly 1 NonBlue, exactly 9 World, exactly 5 Day, exactly 4 Event
check NthDay for exactly 2 Blue, exactly 0 NonBlue, exactly 9 World, exactly 5 Day, exactly 4 Event
check BrownStays for exactly 1 Blue, exactly 1 NonBlue, exactly 9 World, exactly 5 Day, exactly 4 Event

/* On 2nd day, the Blue person should've left */
run {} for exactly 1 Blue, exactly 1 NonBlue, exactly 9 World, exactly 5 Day, exactly 4 Event

/* On 3rd day, both Blue people should've left */
run {} for exactly 2 Blue, exactly 0 NonBlue, exactly 9 World, exactly 5 Day, exactly 4 Event

/* On 4th day, all Blue people should've left */
run {} for exactly 3 Blue, exactly 1 NonBlue, exactly 81 World, exactly 6 Day, exactly 5 Event
