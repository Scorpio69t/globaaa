open util/ordering[Day]

abstract sig Agent {}
sig Blue extends Agent {}
sig NonBlue extends Agent {}

/* Kripke structure for worlds */
sig World {
	-- each world has a set of agents that believe they're 
	-- either blue or not blue
	blues: set Agent,
	nonblues: set Agent,
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

	-- all worlds are possible first day
	World in first.worlds

	-- as we parse the Kripke structure, we won't add worlds or
	-- relations that weren't there on a previous day i.e. transitions
	-- only are destructive in terms of possibilities
	post.worlds in pre.worlds
	post.beliefs in pre.beliefs

	-- someone leaves when they know their eye color in the actual world
	all a : Agent |
		someoneKnows[pre, a] iff a in post.left 

	-- TODO: HANDLE REMOVING WORLDS AND RELATIONS
	-- as we transition, update relations based on agents' beliefs
	all w : pre.worlds | {
		/* remove relations to and from w */
		transitionChange[pre, w] iff post.worlds = pre.worlds - w and (post.beliefs = pre.beliefs - (World->Agent->w + w->Agent->World))
		--not parse[pre, w] implies endConditions2[pre, post, w]
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

			(blueToNonBlue or nonBlueToBlue) iff (w1->a->w2) in first.beliefs
		}
}

/* All agents should be in a world & no two worlds 
 * should have the same number of blue/nonblue people */
pred complete {
	all a : Agent |
		all w : first.worlds | {
			-- agent has to be in a world
			a in w.blues iff a not in w.nonblues
		}
 
	-- worlds are different
	all disj w1, w2 : first.worlds | {
		not (w1.blues = w2.blues and w1.nonblues = w2.nonblues)
	}
}

/* If the Guru speaks or someone in a world is misinformed,
 * update possible worlds/beliefs */
pred transitionChange[pre : Day, w : World] {
	guruSpeaks[pre, w] or misinformed[pre, w]
}

/* Condition for the Guru speaking */
pred guruSpeaks[pre : Day, w : World] {
	pre = first and #(w.blues) = 0
}

/* There's some world connected to w1 and in this world someone
 * incorrecly holds some belief */
pred misinformed[pre : Day, w1 : World] {
	some w2 : pre.worlds |
		some a : Agent |
			pre != first and w1 in Agent.(w2.(pre.beliefs)) and iKnow[w1, pre.beliefs, a] and not actualWorld[a, w1]
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
			(a in w.blues and a in w'.blues) or (a in w.nonblues and a in w'.nonblues)
		}
	}
}

/* Actual world (from a's perspective) */
pred actualWorld[a : Agent, w : World] {
	(a in w.blues and NonBlue = w.nonblues and (Blue - a) = (w.blues - a)) or
	(a in w.nonblues and (NonBlue - a) = (w.nonblues - a) and Blue = w.blues)
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
check noOneKnows for exactly 2 Blue, exactly 0 NonBlue, exactly 4 World, exactly 2 Day, exactly 2 Event

/* On 2nd day, the Blue person should've left */
run {} for exactly 1 Blue, exactly 1 NonBlue, exactly 4 World, exactly 3 Day, exactly 2 Event

/* On 3rd day, both Blue people should've left */
run {} for exactly 2 Blue, exactly 0 NonBlue, exactly 4 World, exactly 4 Day, exactly 3 Event
