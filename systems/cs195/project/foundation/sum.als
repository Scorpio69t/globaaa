open util/ordering[Turn]

/* Create an agent with the number they think they have */
abstract sig Agent {
    number: Int
}{
    number > 0		-- natural numbers
}

sig Anne extends Agent {}
sig Bill extends Agent {}
sig Cath extends Agent {}

/* Each world has all three agents */
sig World {
	-- these sigs might seem redundant, but this allows us to 
	-- view each agents' numbers as attributes
	anne: Int,
	bill: Int,
	cath: Int,
	agents: set Agent	-- should have Anne, Bill, and Cath
}{
	-- one of the numbers has to be the sum of the other two
    add[anne, bill] = cath or
    add[anne, cath] = bill or
    add[bill, cath] = anne

	-- set of agents can only equal 3
	#(agents) = 3

	-- the set of agents must correspond with the ints for
	-- anne, bill, and cath
	some disj a, b, c : Agent | {
		a in Anne and a.number = anne and a in agents
		b in Bill and b.number = bill and b in agents
		c in Cath and c.number = cath and c in agents
	}
}

/* In this problem the turns are: "I don't know my number", 
 * "I don't know my number", ..., "My number is 50". */
sig Turn {
	-- the turn we're on
	Nth: Int,

	-- store possible solutions
	possibilities: set World,

	-- possible solutions that're multiples of 50
	solutions: set World,

	-- all worlds still viable on the Nth turn
    worlds: set World,

	-- agents' beliefs
    beliefs: set (World -> (Agent -> World)),
}

/* Used to transition from Day to Day */
sig Event {
    pre: Turn,
    post: Turn
} {

	-- increment counter as we go
    post.Nth = add[pre.Nth, 1]

	-- as we parse the Kripke structure, we won't add worlds or
    -- relations that weren't there on a previous day i.e. transitions
    -- only are destructive in terms of possibilities
    post.worlds in pre.worlds
    post.beliefs in pre.beliefs
	post.possibilities in pre.possibilities
	post.solutions in pre.solutions

	-- deal with announcements
	all w : pre.worlds | {
		((pre.Nth = 0 and anneKnows[pre, w]) or (pre.Nth = 1 and billKnows[pre, w]) or
		(pre.Nth = 2 and cathKnows[pre, w])) iff w not in post.worlds
	}

	-- after the last announcement, aggregate all worlds where anne
	-- could know her number
	all w : pre.worlds | {
		((pre.Nth >= 3) and anneKnows[pre, w]) iff w in post.possibilities
	}

	-- out of all possible solutions, only those where anne's number is
	-- a multiple of 50 will be viable
	all w : pre.possibilities | {
		(rem[50, w.anne] = 0) iff w in post.solutions
	}

	-- delete relations from worlds no longer viable
    let deleted = pre.worlds - post.worlds | {
        post.beliefs = pre.beliefs - (World->(Agent->deleted) + deleted->(Agent->World))
    }
}

/* Relate all Turns and Events */
fact trace {
    all s: Turn - last | {
        some e: Event |
            e.pre = s and e.post = s.next
    }
}

/* Initial conditions */
fact init {
	-- zeroed out fields
	first.Nth = 0
	no first.possibilities
	no first.solutions

	-- make sure roots of trees exist
	firstRoot

	-- all worlds are different
	diffWorlds[first]

	-- set up belief system between first.worlds
	initialBeliefs[first]

	-- all worlds possible
	World in first.worlds

	/* place any force*tree predicates here */
}

/* For an agent to eventually know their number, it must be of the form
 * (2x, x, x), (x, 2x, x), or (x, x, 2x). As pointed out in the book these
 * values are symmetric to variations of (50, 25, 25) and therefore we can 
 * use them as roots to the trees that encapsulate our kripke structures */
pred firstRoot {
	some disj a, b, c : first.worlds | {
		a.anne = 2
		a.bill = 1
		a.cath = 1	

		b.anne = 1
		b.bill = 2
		b.cath = 1	

		c.anne = 1
		c.bill = 1
		c.cath = 2	
	}
}

/* Avoid redundancy i.e. each world should have a different set of numbers */
pred diffWorlds[t : Turn] {
	all disj w1, w2 : t.worlds | {
		w1.anne != w2.anne or
		w1.bill != w2.bill or
		w1.cath != w2.cath
	}
}

/* Initial belief system where any agent believes in their world and
 * any other world where their number is the sum/difference of the other 
 * two agents' numbers */
pred initialBeliefs[t : Turn] {
	all w1, w2 : t.worlds | 
		all a : Agent | {
			connect[a, w1, w2] iff (w1->a->w2) in t.beliefs
		}	
}

/* Helper called in initialBeliefs. Return true if there is a belief 
 * between two worlds. Note, these two worlds don't have to be disjoint */
pred connect[a : Agent, w1 : World, w2 : World] {
	-- self loop
	(a in w1.agents and a in w2.agents and w1.anne = w2.anne and w1.bill = w2.bill and w1.cath = w2.cath) or

	-- connect worlds that could be the sum/difference of values in your world; if you're in a starting world
	-- the agent with value 2 should only have a self loop
	(a in w1.agents and a in Anne and (not startingWorld[a, w1]) and w2.bill = w1.bill and w2.cath = w1.cath and viableValue[w2.anne, w1.bill, w1.cath]) or
	(a in w1.agents and a in Bill and (not startingWorld[a, w1]) and w2.anne = w1.anne and w2.cath = w1.cath and viableValue[w2.bill, w1.anne, w1.cath]) or
	(a in w1.agents and a in Cath and (not startingWorld[a, w1]) and w2.bill = w1.bill and w2.anne = w1.anne and viableValue[w2.cath, w1.bill, w1.anne])
}

/* Helper called in connect. Check if val1 is the sum or difference 
 * of val2 and val3 */
pred viableValue[val1 : Int, val2 : Int, val3 : Int] {
	val1 = minus[val2, val3] or val1 = minus[val3, val2] or val1 = add[val2, val3]
}

/* Helper called in connect. If we're in a starting world and agent a has value 2,
 * they should know their world is correct */
pred startingWorld[a : Agent, w : World] {
	(a in Anne and w.anne = 2 and w.bill = 1 and w.cath = 1) or
	(a in Bill and w.anne = 1 and w.bill = 2 and w.cath = 1) or
	(a in Cath and w.anne = 1 and w.bill = 1 and w.cath = 2)
}

/* For an agent a, check if their only belief is a self loop 
 * This means they 'know' their number. */
pred anneKnows[t : Turn, w : World] {
	some a : w.agents |
		a in Anne and a.(w.(t.beliefs)) = w
}

pred billKnows[t : Turn, w : World] {
	some a : w.agents |
		a in Bill and a.(w.(t.beliefs)) = w
}

pred cathKnows[t : Turn, w : World] {
	some a : w.agents |
		a in Cath and a.(w.(t.beliefs)) = w
}

/* USE FOR TESTING:
 * Let's try to force certain relations to be more illustrative. This can be used
 * to create a complete tree of depth 2; full enough to notice some changes once agents
 * announce information */
pred forceATree {
	some w : first.worlds | {
		w.anne = 2 and w.bill = 1 and w.cath = 1 and #(Agent.(w.(first.beliefs))) = 3

		all nextDepth : Agent.(w.(first.beliefs)) - w | {
			#(Agent.(nextDepth.(first.beliefs))) = 4

		/* This would add worlds s.t. leaves in the tree wouldn't end up with a self
		 * loop from Anne, however it takes too long for alloy to handle this additional
		 * constraint. */
		//	all nextNextDepth : Agent.(nextDepth.(first.beliefs)) - nextDepth | {
		//		anneKnows[first, nextNextDepth] iff #(Agent.(nextNextDepth.(first.beliefs))) = 3
		//	}
		}
	}
}

/* USE FOR TESTING:
 * Same as above for tree with root (1, 2, 1) */
pred forceBTree {
	some w : first.worlds | {
		w.anne = 1 and w.bill = 2 and w.cath = 1 and #(Agent.(w.(first.beliefs))) = 3

		all nextDepth : Agent.(w.(first.beliefs)) - w | {
			#(Agent.(nextDepth.(first.beliefs))) = 4

		//	all nextNextDepth : Agent.(nextDepth.(first.beliefs)) - nextDepth | {
		//		anneKnows[first, nextNextDepth] iff #(Agent.(nextNextDepth.(first.beliefs))) = 3
		//	}
		}
	}
}

/* USE FOR TESTING:
 * Same as above for tree with root (1, 1, 2) */
pred forceCTree {
	some w : first.worlds | {
		w.anne = 1 and w.bill = 1 and w.cath = 2 and #(Agent.(w.(first.beliefs))) = 3

		all nextDepth : Agent.(w.(first.beliefs)) - w | {
			#(Agent.(nextDepth.(first.beliefs))) = 4

		//	all nextNextDepth : Agent.(nextDepth.(first.beliefs)) - nextDepth | {
		//		anneKnows[first, nextNextDepth] iff #(Agent.(nextNextDepth.(first.beliefs))) = 3
		//	}
		}
	}
}

/* Check that starting worlds exist */
assert startingWorldsExists {
	one w1, w2, w3 : first.worlds | {
		w1.anne = 2
		w1.bill = 1
		w1.cath = 1

		w2.anne = 2
		w2.bill = 1
		w2.cath = 1

		w3.anne = 2
		w3.bill = 1
		w3.cath = 1
	}
}
check startingWorldsExists for exactly 9 World, exactly 9 Agent, exactly 5 Turn, exactly 5 Event, 4 int

run {} for exactly 9 World, exactly 9 Agent, exactly 5 Turn, exactly 5 Event, 4 int
