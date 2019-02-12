% Changes all coordinates of x by a random amount at a random scale chosen
% in an exponential fashion from a range at least [.0001,100]
function out=wideScaleRandomNoiseProposal(x)

  %lpick a radius in the uniform distribution [.0001, 100]
  radius = 10^(rand() * 6 - 4);
  
  out = x + radius * randn(size(x));
end
