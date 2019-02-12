%Traveling Salesman Proposals; let's randomly pick one of our 13 proposals
function out=tsMixProposal(x)

  %get a random number [0, 13]
  r = 13 * rand();

  if r <= 1
      out = salesmanProposal1(x);
  else if r <= 2
      out = salesmanProposal2(x);
  else if r <= 3
      out = salesmanProposal3(x);
  else if r <= 4
      out = salesmanProposal4(x);
  else if r <= 5
      out = salesmanProposal5(x);
  else if r <= 6
      out = salesmanProposal6(x);
  else if r <= 7
      out = salesmanProposal7(x);
  else if r <= 8
      out = salesmanProposal8(x);
  else if r <= 9
      out = salesmanProposal9(x);
  else if r <= 10
      out = salesmanProposal10(x);
  else if r <= 11
      out = salesmanProposal11(x);
  else if r <= 12
      out = salesmanProposal13(x);
  else 
      out = salesmanProposal14(x);
  end
  end
  end
  end
  end
  end
  end
  end
  end
  end
  end
  end
end
