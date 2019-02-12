sig Atom {}

assert union {
	all s: set Atom, p, q : Atom -> Atom | s.(p + q) = s.p + s.q
}
check union for 4 -- feel free to change the bound

assert difference {
	all s : set Atom, p, q : Atom -> Atom | s.(p - q) = s.p - s.q
}
check difference for 2

assert intersection {
	all s : set Atom, p, q : Atom -> Atom | s.(p & q) = s.p & s.q
}
check intersection for 2

/*

WRITE ANSWERS HERE:

For each of the above three identities, say whether it holds,
and if not, give a counterexample (atom names and relation of them).

1. union: holds

2. difference: does not hold:

	Atoms: {Atom0, Atom1}

	r: {'q':(Atom0 -> Atom1), 'p':(Atom1 -> Atom1)}

    So, for example:

		s.p - s.q = Atom1 - Atom1 = the empty set
		s.(p - q) = s.p = Atom1

	which aren't equal.


3. intersection: does not hold:

	Atoms: {Atom0, Atom1}

	r: {'q':(Atom0 -> Atom1), 'p':(Atom1 -> Atom1)}

    So, for example:

		s.p & s.q = Atom1 & Atom1 = Atom1
		s.(p & q) = s.{} = the empty set

	which aren't equal.

*/
