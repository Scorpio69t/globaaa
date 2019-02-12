/* DO NOT EDIT THE STENCIL FILE UNLESS WE ALLOW */

sig Person {handshakes : set Person}
one sig Liar extends Person { }

fact basicDefinitions {
  -- can't shake your own hand
  no iden & handshakes

  -- symmetric
  ~handshakes in handshakes

  -- since handshakes is a set, each binary relation 
  -- is by definition singular
}

pred allUniqueHandshakes {
	all disj a, b : Person |
		#(a.handshakes) != #(b.handshakes)
}

run allUniqueHandshakes for exactly 5 Person, 5 Int

pred oneLiar {
	all disj a, b : Person - Liar |
		#(a.handshakes) != #(b.handshakes)
}

run oneLiar for exactly 5 Person, 5 Int

-- The Liar has to actually shake the same number of hands as some non-Liar.
-- For example, let's say with 5 people we have the relations:
--
--        	{Liar -> Person2, Liar -> Person3, Person1 -> Person3,
--			 Person2 -> Liar, Person2 -> Person3, Person3 -> Liar,
--			 Person3 -> Person1, Person3 -> Person2 }
--
-- where everyone shakes hands with someone except Person0. In this case,
-- Liar actually shakes hands with 2 people, but if we allow Liar to lie,
-- they can say they shook hands with, say, 4 people, meaning everyone
-- shook hands with a unique number of people.


-- an assert will look for any instance that violates this predicate
assert noOtherSolution {
	oneLiar implies #(Liar.handshakes) = 2
}

check noOtherSolution for exactly 5 Person, 5 Int
