%For this problem we use a modified localSearch and additional proposals.

%From our original localSearch.m function we've added a decreasing epsilon 
%s.t. as the code runs we decrease the value of epsilon. Instead of simply 
%returning if after timeDelta seconds we haven't deviated by more than the 
%funcDelta, we check our epsilon and decrease it by half, running 
%localSearch again until epsilon dips below .0001. This provides us with a 
%balance between exploration when we're beginning to optimize and 
%exploitation as we hone in on a final answer.

%Furthermore, if it seems like we get stuck, we'll increase the funcDelta
%for a particular epsilon and then return it to the original for the next 
%epsilon. This guarantees that our optimization will eventually stop.

%Also, we've added a few other proposals:
%
%            wideScaleRandomNoiseLargeCoordinateProposal.ml  
%            wideScaleRandomNoiseRandomCoordinateProposal.ml
%
%what these actually do are in their respective files' comments, but the idea
%behind increasing the number of proposals takes into account what Paul
%said in class when referencing the Netflix challenge: simply increasing
%the number of 'somewhat clever' proposals can make progress towards optimization.

%Taking all this into account, then, we start our code with a somewhat larger 
%epsilon and smaller funcDelta, since both can change in our localSearch.
%As our goal for an overall rating is 41, an epsilon of 1/4 seemed like a
%good starting point.

%Running the code below generally returns ratings between 40-41, though the
%circles saved in bestcircles.mat were compact enough to fit into a square 
%of side length ~39.

function out=arrangeCirclesRunner()
    localSearch(@arrangeCircles,@wideScaleRandomNoiseMix5Proposal,rand([1 20]),.25,0,100,5,0.5)
end