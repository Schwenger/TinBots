classdef TinUtils < handle
    properties (Constant)
        step_size = 0.9;
        % The assumption of centrality (see raycast_to) introduces some
        % error, so the bounds are:
        % [-1/sqrt(2) 1/sqrt(2)]
        error_range = [-1 +1]/sqrt(2);
        radius = 0;%5.3/2;
    end
    
    methods (Static)
        function line = readline(file_handle)
            coder.extrinsic('fgets');
            line = fgets(file_handle);
        end
        
        function path = calculate_path(matrix, dst_x, dst_y, current_x, current_y)
            % inspired by:
            % http://de.mathworks.com/help/robotics/examples/path-planning-in-environments-of-different-complexity.html
            
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
        
        function is_same = same_square(row, col, next_row, next_col)
            is_same = floor(next_row) == floor(row) && floor(next_col) == floor(col);
        end

        function inside = is_inside(pos, rows, cols)
            inside = (pos(1) >= 1) && (pos(1) < cols + 1) && (pos(2) >= 1) && (pos(2) < rows + 1);
        end

        function [distance, object] = raycast_to(matrix, pos, phi, to, QP_vd, QP_q)
            object = matrix(to(2), to(1));
            if object == 0
                distance = inf;
            else
                % Assume the square of the bot is exactly in the middle of the
                % box.  Maximum error: sqrt(2)*0.5 \approx 0.707 (unbiased)
                % A square (i, j) represents the 2D interval $$[i, i+1) \times
                % [j, j+1)$$, with precisely these parens (half-open).
                distance = intersectQuarterSquare(pos(1)+0.5, pos(2)+0.5, phi, to + QP_vd, QP_q);
                distance = max(0, distance - TinUtils.radius);
                if distance >= Inf
                    object = 0;
                end
            end
        end

        function [distance, object] = raycast(matrix, row, col, phi)
            step = [cos(phi) sin(phi)] .* TinUtils.step_size;
            [rows, cols] = size(matrix);
            cur_pos = [col row];
            cur_delta = [0.5 0.5];
            QP_q = sign(step) + (step == 0);
            QP_vd = -QP_q/2 + 0.5;

            % Step until we hit something
            while 1
                % Step until something changes
                assert(all(cur_delta < 1) && all(cur_delta >= 0));
                while all(cur_delta < 1) && all(cur_delta >= 0)
                    cur_delta = cur_delta + step;
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
                    [distance, object] = TinUtils.raycast_to(matrix, [col row], phi, cur_pos + [change(1) 0], QP_vd, QP_q);
                    if object
                        break;
                    end
                end
                if change(2)
                    [distance, object] = TinUtils.raycast_to(matrix, [col row], phi, cur_pos + [0 change(2)], QP_vd, QP_q);
                    if object
                        break;
                    end
                end
                if change(1) && change(2)
                    [distance, object] = TinUtils.raycast_to(matrix, [col row], phi, next_pos, QP_vd, QP_q);
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
