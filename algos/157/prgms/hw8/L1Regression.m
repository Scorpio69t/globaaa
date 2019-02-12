function [f,A,b,Aeq,beq,LB,UB]=L1Regression(x,y)
% Given vectors x, y that represent a series of points in the plane,
% returns parameters f, A, b, Aeq, beq, LB, UB, designed to be used as
% inputs to Matlab's linear programming routine,
%   out = linprog(f,A,b,Aeq,beq,LB,UB)
% where the first two entries of the resulting vector "out" should
% represent the slope and intercept of the line that has minimal total
% distance from the input points.

%in order to run:
%    [f,A,b,Aeq,beq,LB,UB] = L1Regression(x,y);
%    linprog(f,A,b,Aeq,beq,LB,UB)

%add additional variables dependent on size of input vectors
f= [0 0 ones(size(x))];

%let's set up our A matrix
A = [];
sz = size(x); sz = sz(2);
for i = 1:sz
   top= [-x(i) -1 zeros(size(x))]; top(i + 2) = -1;
   bottom= [x(i) 1 zeros(size(x))]; bottom(i + 2) = -1;
   A= [A;top;bottom];
end

%let's set up the bounds of our inequalities
b=[-y;y];
b=b(:)';

%don't think I actually need the rest of these parameters
Aeq=[];
beq=[];
LB=[];
UB=[];
end
