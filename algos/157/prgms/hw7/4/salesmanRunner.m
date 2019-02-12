%In order to get a close to optimal travel distance, we've done a few
%things. First, we've used the localSearch, which we used in parts
%1, 2, and 3, which varies both the epsilon and funcDelta. We do this so
%that we get a nice balance between eploration and exploitation and to
%make sure our optimized code halts (if, for instance, our epsilon was
%large and funcDelta small, there's a chance we'd never run timeDelta
%seconds evaluating each iteration within the funcDelta).

%Taking this into account, the initial epsilon and funcDelta that we choose
%can be seen as upper and lower bounds, respectively. As our localSearch
%runs, the epsilon will decrease on each iteration until we get to epsilon
%< .00000000001 and our funcDelta will increase if we go timeDelta * 4
%seconds without updating our epsilon; this is the 'stuck' condition we
%want to avoid. Each time we decrease our epsilon, we'll return funcDelta
%to its original value. Therefore, values of 5 and .1 for epsilon and
%funcDelta, repsectively, allow us a bit more exploration at the beginning,
%while later on, after we've optimized a bit, we hone in more precisely on a
%final answer.

%We've also written a number of additional proposals. Proposal 1 swaps
%random values. Taking this as inspiration, future proposals, for example
%2, 3, 5, 8, and 11 all swap cities, but instead of swapping single cities
%they swap groups of cities. We also noticed that the position of the first
%and last cities are important. If, for instead, our salesman could start 
%in the smaller group of cities far away from the main cluster and travel to
%all nearby cities before traveling the significant distance connecting 
%the two groups of cities, this would likely be shorter than traveling to
%and from the groups. Multiple proposals of ours (4, 6, 7, & 10) focus on the 
%first and last cities, swapping in others to see if it gives better
%routes.

%And lastly, we 'prime the pump' a bit. In the hw writeup, it mentions that 
%we can look at the locations of the cities and design our code to take
%advantage of this. In the code below, setup() places some of the far away
%cities at the beginning of the salesman's route - this looks to do
%something similar to what is described above i.e. try to minimize the
%times the salesman has to travel to and from the clusters of faraway
%groups.

%Running the code below, we generally get distances between 350-370. Our
%shortest distance, saved in bestsalesman.mat is 352.9.
function out=salesmanRunner()
    initial = (1:100);
    %let's prime our sequence, trying to start in faraway cities
    initial = setup(initial);
    out=localSearch(@travelingSalesman, @tsMixProposal,initial,5,1,100,3,.1);
end

function out=swap(x1, x2, lst)
  tmp = lst(x1);
  lst(x1) = lst(x2);
  lst(x2) = tmp;
  out = lst;
end

%place faraway cities at beginning of route
function out=setup(lst)
    lst = swap(73, 1, lst);
    lst = swap(84, 2, lst);
    lst = swap(86, 3, lst);
    out = lst;
end