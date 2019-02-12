%swap with regions which are far away
function out=salesmanProposal6(x)

    map=[98 86 84 73 70 61 43 33 9 7 4];

    index = round((100-1)*rand()+1);
    mapIndex = round((11-1)*rand()+1);
    if (ismember(x(index),map))
        swap(x,1,search(x,map(mapIndex)));   
    end
    out = x;
end 

function swap(x,i,j)
   temp = x(i);
   x(i) = x(j);
   x(j) = temp;
end

%search current location in the array for city target
function index=search(x,target) 
    for i=1:100
        if (x(i) == target)
           index=i; return;
        end
    end
end

