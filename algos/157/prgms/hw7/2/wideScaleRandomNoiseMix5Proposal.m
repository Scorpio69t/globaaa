%Let's augment our Mix3Proposal with extra proposals
function out=wideScaleRandomNoiseMix5Proposal(x)

  %get a random number [0, 5]
  r = 5 * rand();

  if r <= 1
      out = wideScaleRandomNoiseProposal(x);
  else
      if r <= 2
        out = wideScaleRandomNoiseOneCoordinateProposal(x);
      else
          if r <= 3
            out = wideScaleRandomNoisePairProposal(x);
          else
              if r <= 4
                out = wideScaleRandomNoiseRandomCoordinateProposal(x);
              else
                out = wideScaleRandomNoiseLargeCoordinateProposal(x);
              end
          end
      end
  end

end
