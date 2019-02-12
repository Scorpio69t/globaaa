/* DO NOT EDIT THE STENCIL FILE UNLESS WE ALLOW */

sig State {succ: set State}
sig Initial extends State {}

fact atLeastOneInitial {
	-- one or more initial states
  	some Initial
}

// (a) 
pred deterministic {
	-- only one initial state
  	one Initial

	-- each state has at most one succ
	all s : State | lone s.succ
 
}
run deterministic

// (b) 
pred nondeterministic {
	some s : State | 
		-- we have either multiple initial states
		some Initial and not one Initial or
			-- or multiple successors for a state
			some s.succ and not one s.succ
}
run nondeterministic

// (c)
pred unreachable {
	-- initial state can't be unreachable from itself
	-- https://piazza.com/class/jck2imtit685it?cid=214
	some s : State - Initial | 
		s not in Initial.^succ
}
run unreachable

// (d)
pred reachable {
	all s : State - Initial | 
		s in Initial.^succ 
}
run reachable

// (e)
pred connected {
  	all disj s, t : State |
		t in s.^succ  
}
run connected

// (f)
pred deadlock {
	let deadState = 
		some s : State | 
			s in Initial.^succ and
				no s.succ |
	let deadInitial = 
		no Initial.succ |
	-- I think this covers the case when Initial has no succ
	deadState or deadInitial
}
run deadlock

// (g) 
pred livelock {
	some disj s, t : State |
		let pathMinusTS = t.^(succ - (t -> s)) |
			-- s is reachable from t 
			s in t.succ and
				-- t is in a cycle 
				t in pathMinusTS and
					-- s not in that cycle
					s not in pathMinusTS	
}
run livelock
