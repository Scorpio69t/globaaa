%let's swap 4 cities, starting at random indices; a lot like proposals 2,
%3, 5, and 8
function out=salesmanProposal11(x)
    %generate random numbers
    i1 = round((96 - 1) * rand() + 1);
    i2 = round((96 - 1) * rand() + 1);
    
    %swap 5 values in these intervals
    for i = 1:4
       temp = x(i1 + i); x(i1 + i) = x(i2 + i); x(i2 + i) = temp;
    end
    
    out = x;
end