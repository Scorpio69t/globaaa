% Runs a random one of the three wideScaleRandomNoise proposal functions on
% the input
function out=wideScaleRandomNoiseMix3Proposal(x)
  %get a random number [0, 3]
  r = 3 * rand();

  if r <= 1
      out = wideScaleRandomNoiseProposal(x);
  else
      if r <= 2
        out = wideScaleRandomNoiseOneCoordinateProposal(x);
      else
        out = wideScaleRandomNoisePairProposal(x);
      end
  end

end
