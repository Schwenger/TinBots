%% 'dir' must be in radians
%% Examples:
%% assert(  intersectQuarterSquare(0, 0, 0) == 5  )
%% assert(  intersectQuarterSquare(-1, 0, 0) == 6  )
%% assert(  intersectQuarterSquare(1, 0, 0) == 4  )
%% assert(  intersectQuarterSquare(0, 0, -pi/4) <= 5*sqrt(2) + 1e-10  )
%% assert(isinf(  intersectQuarterSquare(0, 0, +pi/4)   ))
function dist = intersectQuarterSquare(x, y, dir, QP_v, QP_q)

% Specify quarter-square:
% QP_v is the pivotal vertex, QP_q sppecifies the quadrant

% Essential idea: if it hits the square, then it has to hit at least one
% of the (full-)lines where one of the intersection points are in the
% quarter-square.  Only exception: we already are inside the quarter-square.
checkTimes = [0 , whenReaches([x y], [cos(dir) sin(dir)], QP_v)];
invalidX = 1 - isOnHalf(x + checkTimes * cos(dir), QP_v(1), QP_q(1));
invalidY = 1 - isOnHalf(y + checkTimes * sin(dir), QP_v(2), QP_q(2));
invalidT = checkTimes < 0;
inval = invalidX + invalidY + invalidT;

for i = 1:length(checkTimes)
    if inval(i)
        checkTimes(i) = Inf;
    end
end

dist = min(checkTimes);

if isnan(dist)
    [x y dir QP_v QP_q]
end

end

function b = isOnHalf(u, v, q)
    b = ((u - v)*q >= -1e-9) .* ((u - v)*q <= 1+1e-9);
end

function t = whenReaches(x, dx, v)
    % No need to special-case "parallel" and "parallel and identical",
    % since Inf is fine in these cases.
    t = (v-x) ./ dx;
end
