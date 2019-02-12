%let's swap the first and last values
function out = salesmanProposal6(x)
  temp = x(1); x(1) = x(100); x(100) = temp;
  out = x;
end
