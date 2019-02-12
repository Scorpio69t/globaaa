function out=wideScaleRandomNoiseLargeCoordinateProposal(x)
% Changes a random number of coordinates of x, focusing on the larger circles,  
% by a random amount at a random scale chosen in an exponential fashion 
% from a range at least [.0001,100]. The idea here is that if we can optimally 
% place the larger circles, this'll have a greater effect on the overall
% rating.

  %pick a radius in the uniform distribution [.0001, 100]
  radius = 10^(rand() * 6 - 4);

  sz = size(x); sz = sz(2);
  
  %get a random number of indices to change in the upper half range
  n = round((sz - (sz/2 + 1)) * rand() + sz/2 + 1);
  
  %change a random number of components of the vector in the upper range
  for i = n : sz
      x(i) = x(i) + (radius * randn());
  end
  
  out = x;
end