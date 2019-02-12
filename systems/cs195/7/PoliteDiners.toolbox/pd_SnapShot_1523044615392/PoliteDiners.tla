--------------------------- MODULE PoliteDiners----------------------------

EXTENDS Integers


CONSTANTS

    numJudges


VARIABLES

    requests, \* the set of pending chopstick requests
    judgeState, \* judgeState[i] is the state of judge i
    chopState  \* chopState[i] is the state of chopstick i, which is between
               \* judge i and judge i+1 % numJudges


vars == <<judgeState, chopState, requests>>

judges == 0..(numJudges - 1)

chops == 0..(numJudges - 1)

LeftChopstickOf(p) == p

RightChopstickOf(p) == (p + 1) % numJudges

TypeOK == /\ chopState \in [chops -> { "clean", "dirty" }]
          /\ judgeState \in [judges -> SUBSET chops]
          /\ requests \in SUBSET chops

\* all judges except 0 and numJudges - 1 have their right chopstick
Init == /\ judgeState = [x \in 0..(numJudges - 1) |-> { RightChopstickOf(x) }]
        /\ judgeState = [judgeState EXCEPT ![0] = { LeftChopstickOf(0), RightChopstickOf(0) }]
        /\ judgeState = [judgeState EXCEPT ![(numJudges - 1)] = {}]
        /\ chopState = [x \in chops |-> "dirty"]
        /\ requests = {}


Owner(f) == IF f \in judgeState[f] THEN f ELSE (f - 1) % numJudges

Requester(f) == IF f \in judgeState[f] THEN (f - 1) % numJudges ELSE f


RequestLeft ==
    \E j \in judges :
        /\ LeftChopstickOf(j) \notin judgeState[j]
        /\ LeftChopstickOf(j) \notin requests
        /\ requests' = requests \union { LeftChopstickOf(j) }
        /\ UNCHANGED judgeState
        /\ UNCHANGED chopState
       

RequestRight ==
    \E j \in judges :
        /\ RightChopstickOf(j) \notin judgeState[j]
        /\ RightChopstickOf(j) \notin requests
        /\ requests' = requests \union { RightChopstickOf(j) }
        /\ UNCHANGED judgeState
        /\ UNCHANGED chopState

       
HonorRequest ==
    \E r \in requests :
        /\ chopState[r] = "dirty"
        /\ chopState' = [chopState EXCEPT ![r] = "clean"]
        /\ judgeState' = [judgeState EXCEPT ![Owner(r)] = @ \ { r },
                                            ![Requester(r)] = @ \union { r }]
        /\ requests' = requests \ { r }

           
DirtyChops ==
    \E j \in judges :
        /\ RightChopstickOf(j) \in judgeState[j]
        /\ LeftChopstickOf(j) \in judgeState[j]
        /\ chopState[RightChopstickOf(j)] = "clean"
        /\ chopState[LeftChopstickOf(j)] = "clean"
        /\ chopState' = [chopState EXCEPT ![RightChopstickOf(j)] = "dirty",
                                          ![LeftChopstickOf(j)] = "dirty"]
        /\ UNCHANGED requests
        /\ UNCHANGED judgeState


Next == RequestLeft \/ RequestRight \/ HonorRequest \/ DirtyChops

Spec == /\ Init
        /\ [][Next]_vars
        /\ WF_vars(Next)
       

Nonstarvation ==
   \A p \in judges :
      []<>(judgeState[p] = { LeftChopstickOf(p), RightChopstickOf(p) })


=============================================================================
