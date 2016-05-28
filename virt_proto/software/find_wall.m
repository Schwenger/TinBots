function [wall_p, wall_rot] = find_wall(dists)
sens_angles = [-150  -90  -45  -20  +20  +45  +90  +150];
%                1     2    3    4    5    6    7    8
assert(length(dists) == length(sens_angles))
order = [4 3 5 2 6 1 7 8];
rot_angles = sens_angles - (-20);

wall_p = 0;
wall_rot = 0;
wall_dist = Inf;

for i = order
    if dists(i) < wall_dist
        wall_dist = dists(i);
        wall_p = 1;
        wall_rot = rot_angles(i);
    end
end

wall_dist
wall_p
wall_rot

end
