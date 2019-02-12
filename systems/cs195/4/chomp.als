open util/ordering[Board]

// Chomp has two players: P1 and P2
abstract sig Player {}
one sig P1, P2 extends Player {}

// The chomp board has 2 dimensions: rows and columns
abstract sig Dimension {}
one sig Rows, Cols extends Dimension {}

// Each dimension has a set of indices (row 1, row 2, …)
sig Index {}

// A chomp board consists of:
abstract sig Board {
	inplay : Dimension -> set Index, -- the rows/columns not taken yet
	turn : one Player, -- the player taking a turn
  	chomp : lone Dimension -- the player’s choice (rows/columns but not both)
}

// A playable chomp board has at least one square (the soap) left
pred playable[b: Board] { some b.inplay[Rows] and some b.inplay[Cols] }

// Player one starts off the game with a playable (non-empty) board
fact FirstBoard { playable[first] and first.turn = P1 }

/* If there are squares to take, a player must make a move
If a player makes a move, there must be squares to take */
fact DefiniteGameplay { all b:Board | playable[b] iff some b.chomp }

/* All gameplay progress (events) are stated as a relationship between
a game board and it’s successor (except for last, which has no successor!) */
fact NextBoard {  
	all b:Board - last | {
    	-- alternating turns
    	b.next.turn != b.turn

    	-- the game board always stays the same, or gets smaller
   	 	b.next.inplay in b.inplay

    	-- if the board is playable, and there’s a move:
   		some b.chomp implies {
        	-- a player can take rows, but not columns:
            b.chomp in Rows implies {
                    b.inplay[Rows] not in b.next.inplay[Rows]
                    b.inplay[Cols] in b.next.inplay[Cols]
            }

            -- a player can take columns, but not rows:
            b.chomp in Cols implies {
                    b.inplay[Cols] not in b.next.inplay[Cols]
					b.inplay[Rows] in b.next.inplay[Rows]
            }
    }
    -- if there is no move (unplayable), the board stays the same
    no b.chomp implies b.inplay in b.next.inplay
}}

// the winner does not take the soap, which results in an unplayable board
pred winner[p: Player] { some b: Board | playable[b] and not playable[b.next] and b.turn != p }

run {}

pred twoCanWin {
  	-- an instance where player 2 wins
  	winner[P2]
}
run twoCanWin for 10 Board, 5 Index, 5 Int

pred canFinishEarly {
  	-- an instance with more than one unplayable board
  	some b, b' : Board - last |
		not	playable[b] and not playable[b']
}
run canFinishEarly for 10 Board, 5 Index, 5 Int

// for playable boards of this player’s turn
pred WinningStrategy[p: Player] { 

	all b:Board | playable[b] and b.turn = p implies {
		-- if there are more rows than columns
    	b.inplay[Rows] not in b.inplay[Cols] implies {
       		-- return a square board; take a row
            { 

				let size = b.inplay[Rows] - b.inplay[Cols] |
					b.inplay[Cols] = b.next.inplay[Cols] and
			  		b.inplay[Rows]-b.next.inplay[Rows] = size
			}
    	}
    
		-- if there are more columns than rows
    	b.inplay[Cols] not in b.inplay[Rows] implies {
       		-- return a square board; take a column
			{ 
				let size = b.inplay[Cols] - b.inplay[Rows] |
					b.inplay[Rows] = b.next.inplay[Rows] and
			  		b.inplay[Cols]-b.next.inplay[Cols] = size
			}
    	}

    	-- if there are equal rows and columns
    	b.inplay[Cols] = b.inplay[Rows] implies {
        	-- take only one row or column
			{ one b.inplay[Cols]-b.next.inplay[Cols]
  		  	  no b.inplay[Rows]-b.next.inplay[Rows] }
             
			or { one b.inplay[Rows]-b.next.inplay[Rows]
        	 	 no b.inplay[Cols]-b.next.inplay[Cols] }
    	}
	}
}

-- this doesn't seem to work; what if the original board is square?
pred P1StrategyWins { WinningStrategy[P1] implies winner[P1] }
run P1StrategyWorks { P1StrategyWins } for 10 Board, 5 Index, 5 Int
check P1StrategyFails { P1StrategyWins } for 10 Board, 5 Index, 5 Int

-- the starting board has to be non-square and off by a factor of 1
pred StartingBoard { 
	let NumCols = #(first.inplay[Cols]) |
		let NumRows = #(first.inplay[Rows]) | 
			--rem[NumCols + NumRows, 2] = 0
			NumCols = NumRows
}

pred PlayerTwo {
	WinningStrategy[P2]
}

pred reason { WinningStrategy[P1] and StartingBoard and PlayerTwo }
check reasonImpliesP1StrategyFails { reason implies (not P1StrategyWins) } for 10 Board, 5 Index, 5 Int
check P1StrategyFailsImpliesReason { (not P1StrategyWins) implies reason } for 10 Board, 5 Index, 5 Int
