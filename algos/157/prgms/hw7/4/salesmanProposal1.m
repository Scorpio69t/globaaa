%swap two random numbers
function out=salesmanProposal1(x)
    %generate two random numbers
    i1 = round((100 - 1) * rand() + 1);
    i2 = round((100 - 1) * rand() + 1);

    %swap the values at these two indices 
    temp = x(i1); x(i1) = x(i2); x(i2) = temp;

    out = x;
end



