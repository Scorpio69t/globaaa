sig Atom { -- define a unary relation `Atom`
	r: set Atom  -- define a binary relation `r` which is `Atom -> Atom`
}

run {
	some r           -- nonempty
	r.r in r         -- transitive
	no iden & r      -- irreflexive
	~r in r          -- symmetric
	~r.r in iden     -- functional
	r.~r in iden     -- injective
	Atom in r.Atom   -- total
	Atom in Atom.r   -- onto
} for 4

/* PART 1:

r.r in r: 

   The transitive property essentially states that if ('a' -> 'b') and ('b' -> 'c') are in r, 
   then ('a' -> 'c') has to be in r as well. Using the join operator, we can see that given two
   sets of atoms in r, we join based on their inner columns and demand this new relation be
   in r as well. For atoms, this would do something like below:

		('a' -> 'b').('b' -> 'c') = ('a' -> 'c')

    which ensures that if 'a' relates to 'b' and 'b' relates to 'c', 'a' will also relate to 'c', 
    satisfying the transitive property.


no iden & r: 

    An irreflexive relation is one where no element in a set is related to itself. For example,
    if we take the demo from class of cities and roads, we'd want to exclude the case where any 
    city has a road that immediately loops back to itself.

    For any 'a' that's an Atom, ('a' -> 'a') represents the identity relation. We don't want this 
    to be present in r, so we say that amongst all the binary relations in r don't allow those that 
    intersect with iden. Thus, we enforce that no element is related to itself and the relation 
    is therefore irreflexive.


~r in r:

    A symmetric relation is one where ('a' -> 'b') and ('b' -> 'a'). The only tricky thing about 
    this constraint is the '~' operator, which it turns out transposes the relation s.t. for every 
    ('a' -> 'b'), ~('a' -> 'b') = ('b' -> 'a'). The constraint enforces that the inverse of every 
    relation also be in r s.t. for every ('a' -> 'b') in r there's also ('b' -> 'a'), guaranteeing 
    it's symmetric.


~r.r in iden:

   You can usually think of a function as a relation that satisfies the vertical line test; i.e if 
   f(x) = y and f(x) = z then y = z. The above constraint is essentially saying that if we join the 
   transpose of a relation with the relation, we should just get the identity set back. In other
   words, for a particular Atom 'x' in the form ('x' -> _), it should only be in this kind of relation
   for a single, other Atom.

   Concretely, let's assume the contrapositive, i.e. that we don't have a function, but instead have 
   relations ('a' -> 'b') and ('a' -> 'c'). If we join the transpose of these identities we get: 

			('b' -> 'a').('a' -> _) = [('b' -> 'b'), ('b' -> 'c')]
			('c' -> 'a').('a' -> _) = [('c' -> 'b'), ('c' -> 'c')] 

   which aren't identity sets and thus wouldn't be functional by the above definition. With this 
   constaint we'd only ever expect to see at most a single edge **from** any particular atom.


r.~r in iden:

    An injective function is one-to-one s.t. it never maps distinct elements in the domain to the same elements
    in the codomain. We want to join all sets in r with their transpose. This'll give us a set of relations of 
    essentially elements in the domain that map to the same element in the codomain. We want there to be only
    a single element that maps to a particular element in the codomain, and so we assert that the above join
    returns only the identity.

    In other words, if multiple elements mapped to the same element via the relation, then the above join won't 
    just return the identity. For example, given ('b' -> 'a') and ('c' -> 'a') in r, if we take one of these and 
    join it with the transpose of r:

			('b' -> 'a').('a' -> _) = [('b' -> 'b'), ('b' -> 'c')]

    which isn't the identity, meaning the relation isn't injective. 


Atom in r.Atom:

    This kind of relation is left-total, meaning for every Atom 'a' in the set it must be in a relation of the
    form ('a' -> _). The above constraint is saying if we join the set of relations r with a particular atom, 
    it should return a set of Atoms that are in the domain of the relation r. Every Atom must be in this domain,
    so it must be in the left hand column of a relation r.


Atom in Atom.r:

    This kind of relation is onto i.e. surjective. This means that every Atom should be mapped to by some
    relation in r i.e. for every Atom 'a' it should be apart of some relation of the form (_ -> 'a'). For
    any Atom, joining it with r like this: Atom.r, will join an Atom with the leftmost column of relations in
    r, returning simply Atoms in the codomain of these relations i.e. the Atom in the right column of relation 
    r. We then stipulate that every Atom must be in this column. Thus, it has to be of the form (_ -> 'a') meaning
    it has to be mapped to by being in the codomain of some relation.


PART 2:

For each such property eliminated, name the
property and give an example (atom names and relation of them)
that satisfies the rest of the properties

1. Kind of hackish, but if you comment out 'some r', then there's no constraint that there has to be any relations. 
   And, in fact, the empty environment with no atoms satisfies all the constraints, so alloy can find instances.

2. Let's no comment out the transitive property 'r.r in r'.If we do this we get a viable set of relations:

	[(Atom0 -> Atom1), (Atom1 -> Atom0)]

	If this were to satisfy the transitive property, we'd also need to have (Atom0 -> Atom0) and (Atom1 -> Atom1)
	in our set of relations, but those are identity relations, which we explicitly rule out via our irreflexive rule.

3. If we comment out the irreflexive rule the provided instance is trivial: any identity (Atom0 -> Atom0) fulfills
	all the other constraints.

Apart from these 3, we can't comment out a single relation and get all the others to provide viable instances.
*/
