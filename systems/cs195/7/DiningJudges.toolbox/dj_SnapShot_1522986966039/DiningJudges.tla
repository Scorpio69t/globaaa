------------------------- MODULE DiningJudges -------------------------

EXTENDS Integers


CONSTANTS

    numJudges


VARIABLES

    judgeState, \* judgeState[i] is the state of judge i

    chopState  \* chopState[i] is the state of chopstick i, which is between
               \* judge i and judge i+1 % numJudges

                   
vars == <<judgeState, chopState>>

judges == 0..(numJudges - 1)

chopsticks == 0..(numJudges - 1)


TypeOK == /\ judgeState \in [judges -> SUBSET chopsticks]
          /\ chopState \in [chopsticks -> {"available", "taken"}]


Init == /\ judgeState = [x \in judges |-> {}]
        /\ chopState = [x \in chopsticks |-> "available"]

       

\* Use these utilities to determine the indices of the chops for judge j

LeftChopstickOf(j) == j

RightChopstickOf(j) == (j + 1) % numJudges


TakeLeft ==
    \E j \in judges :
        /\ chopState[LeftChopstickOf(j)] = "available"
        /\ LeftChopstickOf(j) \notin judgeState[j]
        /\ chopState' = [chopState EXCEPT ![LeftChopstickOf(j)] = "taken"]
        /\ judgeState' = [judgeState EXCEPT ![j] = judgeState[j] \union {LeftChopstickOf(j)}]


TakeRight == 
    \E j \in judges :
        /\ chopState[RightChopstickOf(j)] = "available"
        /\ RightChopstickOf(j) \notin judgeState[j]
        /\ chopState' = [chopState EXCEPT ![RightChopstickOf(j)] = "taken"]
        /\ judgeState' = [judgeState EXCEPT ![j] = judgeState[j] \union {RightChopstickOf(j)}]
           

DropChops ==
    \E j \in judges :
        /\ judgeState[j] = { LeftChopstickOf(j), RightChopstickOf(j) }
        /\ judgeState[j]' = {} 
        /\ chopState[LeftChopstickOf(j)]' = "taken"
        /\ chopState[RightChopstickOf(j)]' = "taken"


\* The initial model of the problem

Next == TakeLeft \/ TakeRight \/ DropChops

Spec == /\ Init

        /\ [][Next]_vars


=============================================================================
