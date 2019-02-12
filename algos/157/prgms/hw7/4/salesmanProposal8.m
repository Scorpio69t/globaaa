%let's swap 3 cities, starting at random indices; a lot like proposals 2,
%3, 5, and 11
function out=salesmanProposal8(x)
    %generate random numbers
    i1 = round((97 - 1) * rand() + 1);
    i2 = round((97 - 1) * rand() + 1);
    
    %swap 3 values in these intervals
    for i = 1:3
       temp = x(i1 + i); x(i1 + i) = x(i2 + i); x(i2 + i) = temp;
    end
    
    out = x;
end