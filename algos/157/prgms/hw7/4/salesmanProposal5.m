%similar to proposals 2,3, and 8; swap 20 cities starting at random indices
function out=salesmanProposal5(x)
    %generate random numbers
    i1 = round((80 - 1) * rand() + 1);
    i2 = round((80 - 1) * rand() + 1);
    
    %swap 20 values in these intervals
    for i = 1:20
       temp = x(i1 + i); x(i1 + i) = x(i2 + i); x(i2 + i) = temp;
    end
    
    out = x;
end