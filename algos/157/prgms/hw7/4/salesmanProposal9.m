%let's swap the first 5 values with the last 5
function out = salesmanProposal9(x)
  temp = x(1); x(1) = x(100); x(100) = temp;
  temp = x(2); x(2) = x(99); x(99) = temp;
  temp = x(3); x(3) = x(98); x(98) = temp;
  temp = x(4); x(4) = x(97); x(97) = temp;
  temp = x(5); x(5) = x(96); x(96) = temp;
  out = x;
end