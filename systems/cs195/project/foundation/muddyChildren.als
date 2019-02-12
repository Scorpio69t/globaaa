open util/ordering[State]

abstract sig Agent {}

sig Muddy extends Agent {}
sig Clean extends Agent {}

sig World {
	muddy: set Agent,
	clean: set Agent,
}

/* Temporal ordering */
sig State {
	Nth: Int,

	-- those that know if they're muddy or not will leave
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
	pre: State,
	post: State
} {
	-- increment counter as we go
	post.Nth = add[pre.Nth, 1]

	-- as we parse the Kripke structure, we won't add worlds or
	-- relations that weren't there on a previous day i.e. transitions
	-- only are destructive in terms of possibilities
	post.worlds in pre.worlds
	post.beliefs in pre.beliefs

	-- someone steps forward when they know if they're muddy or not in the actual world
	all a : Agent |
		someoneKnows[pre, a] iff a in post.left 

	-- as we transition, update deleted worlds based on agents' beliefs
	all w : pre.worlds | {
		transitionChange[pre, w] iff w not in post.worlds
	} 

	-- delete relations to/from deleted worlds
	let deleted = pre.worlds - post.worlds | {
		post.beliefs = pre.beliefs - (World->(Agent->deleted) + deleted->(Agent->World))
	}
}

fact trace {
    --(relate all States and Events)
	all s: State - last | {
		some e: Event | e.pre = s and e.post = s.next
	} 
}

/* Set up initial belief system */
fact init {
	first.Nth = 0
	no first.left			-- no one knows if they're muddy initially
	initialBeliefs
	complete
	World in first.worlds		-- all worlds are possible first day
}

/* Each agent believes in their current world and any world that
 * deviates by only their muddiness*/
pred initialBeliefs {
	all disj w1, w2 : first.worlds |
		all a : Agent | {
			-- self loop; each agent believes in their world possible
			(w1->a->w1) in first.beliefs

			-- each agent believes in a world where only their muddiness state is different
			let muddyToClean = (a in w1.muddy and w2.clean = (w1.clean + a) and w2.muddy = (w1.muddy - a)) |
			let cleanToMuddy = (a in w1.clean and w2.muddy = (w1.muddy + a) and w2.clean = (w1.clean - a)) |

			(muddyToClean or cleanToMuddy) iff (w1->a->w2) in first.beliefs
		}
}

/* All agents should be in a world & no two worlds 
 * should have the same number of muddy/clean people */
pred complete {
	all a : Agent |
		all w : World | {
			-- agent has to be in a world
			a in w.muddy iff a not in w.clean
		}
 
	-- worlds are different
	all disj w1, w2 : World | {
		not (w1.muddy = w2.muddy and w1.clean = w2.clean)
	}
}

/* If the father speaks or someone in a world is misinformed,
 * update possible worlds/beliefs */
pred transitionChange[pre : State, w : World] {
	fatherSpeaks[pre, w] or misinformed[pre, w] or knowersLeft[pre, w]
}

/* Condition after the father speaks */
pred fatherSpeaks[pre : State, w : World] {
	pre = first and #(w.muddy) = 0
}

/* There's some world connected to w1 and in this world someone
 * incorrecly holds some belief */
pred misinformed[pre : State, w1 : World] {
	some w2 : pre.worlds |
		some a : Agent |
			pre != first and w1 in Agent.(w2.(pre.beliefs)) and iKnow[w1, pre.beliefs, a] and not actualWorld[a, w1]
}


/* Must get rid of worlds where knowers would be uncertain */
pred knowersLeft[pre : State, w : World] {
	some a: pre.left |
		not iKnow[w, pre.beliefs, a]
}

/* Know if you're muddy if you can only see into other worlds that 
 * confirm your beliefs AND current world conforms to actual world */
pred someoneKnows[pre : State, a : Agent] {
	some w : pre.worlds |
		iKnow[w, pre.beliefs, a] and actualWorld[a, w]
}


/* I can only see into other worlds that confirm my beliefs */
pred iKnow[w : World, b : (World -> (Agent -> World)), a : Agent] {
	let worldsFromW = a.(w.b) | {
		all w' : worldsFromW | {
			-- only see worlds that confirm belief
			(a in w.muddy and a in w'.muddy) or (a in w.clean and a in w'.clean)
		}
	}
}

/* Actual world (from a's perspective) */
pred actualWorld[a : Agent, w : World] {
	(a in w.muddy and Clean = w.clean and (Muddy - a) = (w.muddy - a)) or
	(a in w.clean and Muddy = w.muddy and (Clean - a) = (w.clean - a))
}

/* Muddy children know they're muddle on state n. They've therefore
 * stepped forward on state n + 1 */
assert NthTime {
	all s: State | s.Nth = add[#Muddy, 1] implies Muddy in s.left
}

/* Similar predicate to pass to correspondence */
pred correspondenceNthTime {
	all s: State | s.Nth = add[#Muddy, 1] implies Muddy in s.left
}

/* After the muddy children step forward it takes a transition state for the clean
 * children to update their beliefs and then, on the following day, they
 * should've left as well. */ 
assert NPlusOne {
	all s: State | all c: Clean | s.Nth = add[#Muddy, 3] implies c in s.left
}

/* This should be false. Use only in correspondence */
pred correspondenceCleanStays {
        -- there's no state where clean children have left
        all s: State | all c: Clean | c not in s.left
}

check NthTime for exactly 2 Muddy, exactly 0 Clean, exactly 4 World, exactly 8 State, 7 Event
check NthTime for exactly 1 Muddy, exactly 2 Clean, exactly 8 World, exactly 8 State, 7 Event
check NthTime for exactly 2 Muddy, exactly 1 Clean, exactly 8 World, exactly 8 State, 7 Event

check NPlusOne for exactly 2 Muddy, exactly 0 Clean, exactly 0 World, exactly 8 State, 7 Event
check NPlusOne for exactly 1 Muddy, exactly 2 Clean, exactly 8 World, exactly 8 State, 7 Event
check NPlusOne for exactly 2 Muddy, exactly 1 Clean, exactly 8 World, exactly 8 State, 7 Event

run {} for exactly 1 Muddy, exactly 2 Clean, exactly 8 World, exactly 8 State, 7 Event
