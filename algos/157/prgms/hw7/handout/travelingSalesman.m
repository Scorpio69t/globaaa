function total=travelingSalesman(order)
% Given a permutation of the numbers 1, 2, ..., 100, compute the total distance
% a "traveling salesman" would take, visiting 100 American cities in this
% order. Latitude and longitude data is loaded from "cities.mat".

persistent locs lastCalled
%error checking:
if numel(order)~=100 error('Input must have 100 numbers'); end
if any(sort(order(:))~=(1:100)') error('Input must be a permutation of the numbers 1 to 100'); end
if isempty(locs), load('cities'); end %load data for the first time
if isempty(lastCalled) lastCalled=0; end %initialize the time we were last called

total=sum(sqrt(sum(diff(locs(order,:)).^2,2))); %compute the return value!

%graphics code follows
if (now-lastCalled)*60*60*24>0.1, %have we been called in the last tenth of a second?
    lastCalled=now;
    if any(100==get(0,'Children')), ax=get(100,'Children'); else ax=[]; end
    if ~isempty(ax) && isfield(get(ax(1),'UserData'),'travelingSalesman')
        l=get(ax(1),'Children'); set(l,'XData',-locs(order,2)); set(l,'YData',locs(order,1));
    else
        figure(100); clf
        plot(-locs(order,2),locs(order,1),'b:',-locs(:,2),locs(:,1),'r*'); axis equal;
        userData=[]; userData.travelingSalesman=1; set(gca,'UserData',userData);
    end
    drawnow;
end
