open GoatsWolves as gw
open PetsOwners as po

one sig Correspondence {
    ofState: gw/State -> po/State
}

fact {
    -- Define the correspondence of states inductively
    -- Want a relation of the form:
    -- {gw/State0 -> po/State0, gw/State1 -> po/State1, ...}

	-- really cool -- think of this as induction and this is the base case
    gw/first->po/first in Correspondence.ofState
    all gwS: gw/State - gw/last | all poS: po/State - po/last | {
        (gwS -> poS) in Correspondence.ofState implies (gwS.next -> poS.next) in Correspondence.ofState
    }
}

run {} for 12 but exactly 3 Pet, exactly 3 Owner

fun poCount[characterType: set po/Character, state: one po/State, side: one po/Side] : one Int {
    #{c: characterType | state.sideof[c] = side}
}

fun gwCount[characterType: set gw/Character,
                    state: one gw/State,
                    side: one gw/Side] : one Int {
    #{c: characterType | state.sideof[c] = side}
}

pred correspondenceOfState[gwS: gw/State, poS: po/State] {
    -- What correspondence is there between the character-side counts in each problem state?
    gwCount[gw/Goat, gwS, gw/Near] = poCount[po/Owner, poS, po/Near]
    gwCount[gw/Wolf, gwS, gw/Near] = poCount[po/Pet, poS, po/Near]
    gwCount[gw/Goat, gwS, gw/Far]  = poCount[po/Owner, poS, po/Far]
    gwCount[gw/Wolf, gwS, gw/Far]  = poCount[po/Pet, poS, po/Far]
}

pred correspondence {
    -- All states in the relation correspond
    all gwS: gw/State | all poS: po/State | {
            gwS->poS in Correspondence.ofState implies correspondenceOfState[gwS, poS]
    }
}

run vacuityCheckPO {po/solvePuzzle and correspondence} for 12 but exactly 3 Pet, exactly 3 Owner
run vacuityCheckGW {gw/solvePuzzle and correspondence} for 12 but exactly 3 Goat, exactly 3 Wolf

assert POimpliesGW { (po/solvePuzzle and correspondence) => gw/solvePuzzle }
check POimpliesGW for 12 but exactly 3 Pet, exactly 3 Owner -- expect to pass

assert GWimpliesPO { (gw/solvePuzzle and correspondence) => po/solvePuzzle }
check GWimpliesPO for 12 but exactly 3 Pet, exactly 3 Owner -- expect to fail
