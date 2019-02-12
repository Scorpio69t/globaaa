/*
IMPORTANT: Do not edit anything besides the lines next to
           `EDIT NEXT LINE` comments
*/


sig Atom {
	r: set Atom
}

-- NonEmpty

pred NonEmpty {
	some x, y: Atom | x -> y in r
}

assert ReformulateNonEmptyOK {
	(some r) iff NonEmpty
}
check ReformulateNonEmptyOK

-- Transitive

pred ReformulateTransitive {
	all x, y, z: Atom | x -> y in r and y -> z in r implies x -> z in r
}

assert ReformulateTransitiveOK {
	(r.r in r) iff ReformulateTransitive
}
check ReformulateTransitiveOK

-- Irreflexive

pred ReformulateIrreflexive {
	all x : Atom | x -> x not in r
}

assert ReformulateIrreflexiveOK {
	(no iden & r) iff ReformulateIrreflexive
}
check ReformulateIrreflexiveOK

-- Symmetric

pred ReformulateSymmetric {
	all x, y : Atom | x -> y in r implies y -> x in r
}

assert ReformulateSymmetricOK {
	(~r in r) iff ReformulateSymmetric
}
check ReformulateSymmetricOK

-- Functional

pred ReformulateFunctional {
	all x, y, z : Atom | x -> y in r and x -> z in r implies y = z
}

assert ReformulateFunctionalOK {
	(~r.r in iden) iff ReformulateFunctional
}
check ReformulateFunctionalOK

-- Injective

pred ReformulateInjective {
	all x, y, z : Atom | x -> y in r and z -> y in r implies x = z
}

assert ReformulateInjectiveOK {
	(r.~r in iden) iff ReformulateInjective
}
check ReformulateInjectiveOK

-- Total

pred ReformulateTotal {
	all x : Atom | some y : Atom | x -> y in r
}

assert ReformulateTotalOK {
	(Atom in r.Atom) iff ReformulateTotal
}
check ReformulateTotalOK

--Onto

pred ReformulateOnto {
	all x : Atom | some y : Atom | y -> x in r
}

assert ReformulateOntoOK {
	(Atom in Atom.r) iff ReformulateOnto
}
check ReformulateOntoOK
