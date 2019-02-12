function out=wideScaleRandomNoiseRandomCoordinateProposal(x)
% Changes a random number of coordinates of x by a random amount at a 
% random scale chosen in an exponential fashion from a range at least 
% [.0001,100]

  %pick a radius in the uniform distribution [.0001, 100]
  radius = 10^(rand() * 6 - 4);

  sz = size(x); sz = sz(2);
  
  %get a random number of indices to change
  n = round((sz - 1) * rand() + 1);
  
  %change a random number of components of the vector
  for i = 1 : n
      x(i) = x(i) + (radius * randn());
  end
  
  out = x;
end