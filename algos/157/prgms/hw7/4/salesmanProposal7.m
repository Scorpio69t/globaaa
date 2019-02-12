%swap a random index with the first value; a lot like proposal 4
function out=salesmanProposal7(x)
    i = round((100 - 1) * rand() + 1);
    temp = x(i); x(i) = x(1); x(1) = temp;
    out = x;
end