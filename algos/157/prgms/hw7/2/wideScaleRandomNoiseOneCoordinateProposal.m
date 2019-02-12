function out=wideScaleRandomNoiseOneCoordinateProposal(x)
% Changes a random coordinate of x by a random amount at a random scale
% chosen in an exponential fashion from a range at least [.0001,100]

  %pick a radius in the uniform distribution [.0001, 100]
  radius = 10^(rand() * 6 - 4);

  sz = size(x); sz = sz(2);
  
  %get a random index into vector [1, sz]
  i = round((sz - 1) * rand() + 1);
  
  %change only a single component of the vector
  x(i) = x(i) + (radius * randn()); out = x;
end