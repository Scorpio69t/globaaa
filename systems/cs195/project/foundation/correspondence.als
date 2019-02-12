open blueEyes as be
open muddyChildren as mc

one sig Correspondence {
    ofState: be/Day -> mc/State
}

/* Connect the states of the two logic puzzles */
fact {
	-- define this inductively
	be/first -> mc/first in Correspondence.ofState

    all beD: be/Day - be/Day | all mcS: mc/State - mc/last | {
        (beD -> mcS) in Correspondence.ofState iff (beD.next -> mcS.next) in Correspondence.ofState
    }
}

/* If there are the same number of blue eyed people and muddy children, these logic puzzles
 * are the same -- each group recognizes their features on the same day */
assert sameDay { be/correspondenceNthDay => mc/correspondenceNthTime }
check sameDay for 
	exactly 2 be/Blue, exactly 0 be/NonBlue, exactly 8 be/World, 
	 	exactly 3 be/Day, exactly 3 be/Event, exactly 2 mc/Muddy, 
			exactly 0 mc/Clean, exactly 4 mc/World, exactly 3 mc/State, 
				exactly 3 mc/Event

/* This should evaluate to false. Give each model a few extra days. */
assert nextDay { be/correspondenceBrownStays => mc/correspondenceCleanStays }
check nextDay for 
	exactly 1 be/Blue, exactly 1 be/NonBlue, exactly 8 be/World, 
	 	exactly 5 be/Day, exactly 5 be/Event, exactly 1 mc/Muddy, 
			exactly 1 mc/Clean, exactly 4 mc/World, exactly 5 mc/State, 
				exactly 5 mc/Event
