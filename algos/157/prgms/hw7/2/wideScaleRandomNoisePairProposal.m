function out=wideScaleRandomNoisePairProposal(x)
% Changes two random coordinates 2i-1 and 2i of x by a random amount at a
% random scale chosen in an exponential fashion from a range at least
% [.0001,100]

  %pick a radius in the uniform distribution [.0001, 100]
  radius = 10^(rand() * 6 - 4);
  
  %find the size of the input vector
  sz = size(x); sz = sz(2);
  
  %get a random index into vector [1, sz/2]
  i = (round((sz/2 - 1) * rand() + 1));
  
  %change only a single component of the vector
  x(2*i - 1) = x(2*i - 1) + (radius * randn()); 
  x(2*i) = x(2*i) + (radius * randn()); 
  out = x;
end

