%let's swap a value from the middle into the beginning
function out = salesmanProposal10(x)
  temp = x(1);
  x(1) = x(50);
  x(50) = temp;
  out = x;
end