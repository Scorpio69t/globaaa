%let's swap 5 cities, starting at random indices
function out=salesmanProposal2(x)
    %generate random numbers
    i1 = round((95 - 1) * rand() + 1);
    i2 = round((95 - 1) * rand() + 1);
    
    %swap 5 values in these intervals
    for i = 1:5
       temp = x(i1 + i); x(i1 + i) = x(i2 + i); x(i2 + i) = temp;
    end
    
    out = x;
end
