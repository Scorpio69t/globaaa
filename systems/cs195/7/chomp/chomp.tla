 -------------------------------- MODULE chomp -------------------------------

EXTENDS Integers

CONSTANTS
   
    number_rows,
    number_cols

VARIABLES

    row, \* The current number of rows on the board
    col, \* The current number of columns on the board
    turn \* The current turn


vars == <<row, col, turn>>


\* Ensure types of all variables are correct

TypeOK == /\ (turn = 1 \/ turn = 2) 
          /\ (row >= 0 /\ row <= number_rows)
          /\ (col >= 0 /\ col <= number_cols)


\* TODO: Design the initial game state in terms of your variables

Init == /\ turn = 1
        /\ row = number_rows
        /\ col = number_cols


\* TODO: Describe the square player's turn

NextSquare == /\ turn = 1
              \* shouldn't have an empty board; this should always be a square board
              /\ (row /= 0 /\ col /= 0)
              \* either choose a number of rows or cols resulting in square board
              /\ CASE (col > row) -> (col' = row /\ row' = row /\ turn' = 2)
                   [] (row > col) -> (row' = col /\ col' = col /\ turn' = 2)


\* TODO: Describe the random player's turn

NextRandom == /\ turn = 2
              \* player two should always be given a square board 
              /\ (row /= 0 /\ col /= 0)
              \* randomly choose row or column
              /\ (\E choice \in 0..1:
                   \* randomly choose how many to chomp
                    \E rand \in 1..row:
                       CASE choice = 0 -> row' = (row - rand) /\ turn' = 1 /\ UNCHANGED col
                         [] choice = 1 -> col' = (col - rand) /\ turn' = 1 /\ UNCHANGED row) 
           

\* Describes the turns that may take place at each step
\* You need not modify this definition

Next == NextSquare \/ NextRandom


\* A temporal formula defining the gameplay
\* You need not modify this definition

Spec == /\ Init
        /\ [][Next]_vars
        /\ WF_vars(Next) \* Weak fairness ensures action Next is always taken


\* TODO: Define a proposition which determines whether the square
\* player has a winning strategy

\* once a player takes the last row or column, it'll transition to the next
\* player's turn, but neither NextSquare nor NextRandom will be satisfiable.
\* it'll therefore stutter on this player's turn -- this has to be player 1
Prop == <>[](turn = 1)	

===========================================================================

