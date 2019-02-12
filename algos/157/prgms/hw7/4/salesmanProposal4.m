%swap a random index with the last value; idea behind this is that the
%endpoints can have a huge effect on total travel time
function out=salesmanProposal4(x)
    i = round((100 - 1) * rand() + 1);
    temp = x(i); x(i) = x(100); x(100) = temp;
    out = x;
end