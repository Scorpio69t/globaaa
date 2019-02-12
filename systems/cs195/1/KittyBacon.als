sig Cat {
	friends : set Cat
}

-- special Cat; only one of these
one sig KittyBacon extends Cat {
	connectionsOf : set Cat
}

-- every cat must know a cat; problem is, this doesn't preclude a cat
-- from being friends with itself
fact NoLonelyKittens {
	all c : Cat | #c.friends > 0
}

fact OutsideFriends {
	all c : Cat | c not in c.friends
}

fact TwoWayStreet {
	all c, c' : Cat | c in c'.friends iff c' in c.friends
}

-- the set of Cats - KittyBacon (so every other Cat) is in KittyBacon's
-- connections
pred ConnectedKittyBacon {
	Cat - KittyBacon = KittyBacon.connectionsOf
}

-- I think the order matters here, since if you subtract out KittyBacon before possibly adding him 
-- in via .friends and .friends.friends you won't get proper exclusion
fact {
	KittyBacon.connectionsOf = 
		(KittyBacon.friends + KittyBacon.friends.friends + KittyBacon.friends.friends.friends) - KittyBacon
}

pred SuperConnected {
	Cat - KittyBacon in KittyBacon.^friends
}

pred ConnectedKittyBacon_equals_SuperConnected {
	SuperConnected iff ConnectedKittyBacon
}

pred failurePropertyA {not ConnectedKittyBacon_equals_SuperConnected}

pred inexactReason {
	some c : Cat - KittyBacon | c not in KittyBacon.connectionsOf
}

-- this works, but it seems to just be a rephrased way of saying .connectionsOf
pred reasonA { 
	some c : Cat | c not in KittyBacon.friends and c not in KittyBacon.friends.friends and c not in KittyBacon.friends.friends.friends
 }

-- essentially same as above: for a cat in the set, KittyBacon isn't within 4 degrees of separation but
-- it is connected to the set of cats
pred reasonA2 {
	some c : Cat - KittyBacon | KittyBacon not in c.friends and KittyBacon not in c.friends.friends and
	KittyBacon not in c.friends.friends and KittyBacon in c.^friends
}

pred failurePropertyB {KittyBacon not in KittyBacon.connectionsOf}

pred reasonB {
	-- #KittyBacon.friends > 0
	some c : Cat | c in KittyBacon.friends -- this is essentially the same thing 
}

check validateReasonA {failurePropertyA implies reasonA} for exactly 5 Cat

check validateReasonA2 {failurePropertyA implies reasonA2} for exactly 5 Cat

-- this will try to return counterexamples; if it does, failurePropertyB is not equivalent to reasonB
check validateReasonB {failurePropertyB iff reasonB} for exactly 5 Cat

-- generic run
run {}

-- let's check our fact above
run ConnectedKittyBacon

-- run checking both our predicates; it should always be true, so you should never be able
-- to find counterexamples
run {not ConnectedKittyBacon_equals_SuperConnected} for exactly 3 Cat

-- with only .friends + .friends.friends, this won't always be true since you could have 
-- cats 4 degrees of separation away from KittyBacon not in their connectionsOf; this should
-- work after adding .friends.friends.friends
run {not ConnectedKittyBacon_equals_SuperConnected} for exactly 4 Cat

-- this should still not work
run {not ConnectedKittyBacon_equals_SuperConnected} for exactly 5 Cat

-- so KittyBacon isn't completely connected and we have an inexact reason
run AFailsGlobally {failurePropertyA and not inexactReason} for exactly 5 Cat

run BFailsGlobally {failurePropertyB} for exactly 5 Cat

