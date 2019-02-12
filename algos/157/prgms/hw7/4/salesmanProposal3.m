%let's swap pairs of numbers, starting at random indices
function out=salesmanProposal3(x)
    %generate random numbers
    i1 = round((98 - 1) * rand() + 1);
    i2 = round((98 - 1) * rand() + 1);
    
    %swap 2 values in these intervals
    for i = 1:2
       temp = x(i1 + i); x(i1 + i) = x(i2 + i); x(i2 + i) = temp;
    end
    
    out = x;
end