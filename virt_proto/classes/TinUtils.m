classdef TinUtils < handle
    properties (Constant)
        step_size = 0.9;
        radius = 5.3/2;
        pickup_distance = 5.3/2 + 2;
    end
    
    methods (Static)
        function line = readline(file_handle)
            coder.extrinsic('fgets');
            line = fgets(file_handle);
        end
        
        function path = calculate_path(matrix, dst_x, dst_y, current_x, current_y)
            % inspired by:
            % http://de.mathworks.com/help/robotics/examples/path-planning-in-environments-of-different-complexity.html
            
            %matrix(floor(current_y), floor(current_x)) = 0;
            %matrix(floor(dst_y), floor(dst_x)) = 0;
            
            
            grid = robotics.BinaryOccupancyGrid(matrix, 1);
            
            % inflate walls with robot radius
            inflate(grid, 2);
            
            
            start_location = [floor(current_x) floor(current_y)];
            end_location = [floor(dst_x) floor(dst_y)];
            
            prm = robotics.PRM;
            prm.Map = grid;
            
            path = findpath(prm, start_location, end_location);
            
            counter = 0;
            
            while isempty(path) && counter < 20
                prm.NumNodes = prm.NumNodes + 20;
                update(prm);
                path = findpath(prm, start_location, end_location);
                counter = counter + 1;
            end
                        
            show(prm);
        end
        
        function l = get_path_length(path)
            l = 1;
            l = length(path);
        end
        
        function c = get_path_point(path, i, x)
            c = 3.1415;
            c = path(i, x);
        end
        
        function i = convert_path_point(p)
            i = 3.1415;
            i = p;
        end
            
        function is_same = same_square(row, col, next_row, next_col)
            is_same = floor(next_row) == floor(row) && floor(next_col) == floor(col);
        end

        function inside = is_inside(pos, rows, cols)
            inside = (pos(1) >= 1) && (pos(1) < cols + 1) && (pos(2) >= 1) && (pos(2) < rows + 1);
        end

        function [distance, object] = raycast_to(matrix, pos, phi, to, QP_vd, QP_q, limit_distance)
            object = matrix(to(2), to(1));
            if object == 0
                distance = inf;
            else
                distance = intersectQuarterSquare(pos(1), pos(2), phi, to + QP_vd, QP_q);
                distance = max(0, distance - TinUtils.radius);
                if distance >= limit_distance
                    object = 0;
                    distance = Inf;
                end
            end
        end

        function limit_steps = max_steps(limit_distance)
            limit_steps = 2 + (limit_distance + TinUtils.radius) / TinUtils.step_size;
        end

        function [distance, object] = raycast_polymorphic(matrix, row, col, phi, limit_distance)
            step = [cos(phi) sin(phi)] .* TinUtils.step_size;
            [rows, cols] = size(matrix);
            cur_pos = floor([col row]);
            cur_delta = [col row] - cur_pos;
            QP_q = sign(step) + (step == 0);
            QP_vd = -QP_q/2 + 0.5;
            done_steps = 0;
            limit_steps = TinUtils.max_steps(limit_distance);

            % Step until we hit something
            while 1
                % Step until something changes
                assert(all(cur_delta < 1) && all(cur_delta >= 0));
                while all(cur_delta < 1) && all(cur_delta >= 0)
                    cur_delta = cur_delta + step;
                    done_steps = done_steps + 1;
                end

                % Are we too far away anyway?
                if done_steps > limit_steps
                    distance = inf;
                    object = 0;
                    break;
                end

                % Determine what changed
                assert(all(cur_delta < 2) && all(cur_delta >= -1), 'A step too far');
                change = sign(floor(cur_delta));
                assert(sum(abs(change)) <= 2);

                % Check if we WILL (future!) be still inside after
                % resolving deltas
                next_pos = cur_pos + change;
                if ~TinUtils.is_inside(next_pos, rows, cols)
                    distance = inf;
                    object = 0;
                    break;
                end

                % Determine if there's an object somewhere
                if change(1)
                    [distance, object] = TinUtils.raycast_to(matrix, [col row], phi, cur_pos + [change(1) 0], QP_vd, QP_q, limit_distance);
                    if object
                        break;
                    end
                end
                if change(2)
                    [distance, object] = TinUtils.raycast_to(matrix, [col row], phi, cur_pos + [0 change(2)], QP_vd, QP_q, limit_distance);
                    if object
                        break;
                    end
                end
                if change(1) && change(2)
                    [distance, object] = TinUtils.raycast_to(matrix, [col row], phi, next_pos, QP_vd, QP_q, limit_distance);
                    if object
                        break;
                    end
                end

                % Actually move
                cur_pos = next_pos;
                cur_delta = cur_delta - change;
            end
        end
    end
end
