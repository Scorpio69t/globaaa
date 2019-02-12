%let's swap 10 cities, starting at random indices; similar to numerous
%previous proposals
function out=salesmanProposal14(x)
    %generate random numbers
    i1 = round((90 - 1) * rand() + 1);
    i2 = round((90 - 1) * rand() + 1);
    
    %swap 10 values in these intervals
    for i = 1:10
       temp = x(i1 + i); x(i1 + i) = x(i2 + i); x(i2 + i) = temp;
    end
    
    out = x;
end