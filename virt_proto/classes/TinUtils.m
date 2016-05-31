classdef TinUtils < handle
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
                
        function [distance, object] = raycast_hv(matrix, row, col, delta_row, delta_col)
            [rows, cols] = size(matrix);
            next_row = row + delta_row;
            next_col = col + delta_col;
            if (next_row < 1) || (next_row > rows) || (next_col < 1) || (next_col > cols)
                object = 1;
                distance = 0;
            else
                while matrix(next_row, next_col) == 0
                    next_row = next_row + delta_row;
                    next_col = next_col + delta_col;
                    if (next_row < 1) || (next_row > rows) || (next_col < 1) || (next_col > cols)
                        next_row = next_row - delta_row;
                        next_col = next_col - delta_col;
                        break;
                    end
                end
                object = matrix(next_row, next_col);
                distance = delta_row * (next_row - row) + delta_col * (next_col - col) - 0.5;
            end
        end
        
        function [distance, object] = raycast_diag(matrix, row, col, delta_row, delta_col)
            [rows, cols] = size(matrix);
            row = row + 0.5;
            col = col + 0.5;
            next_row = row + delta_row;
            next_col = col + delta_col;
            delta_row = delta_row * 2;
            delta_col = delta_col * 2;
            if (next_row < 1) || (next_row > rows) || (next_col < 1) || (next_col > cols)
                object = 1;
                distance = inf;
            else
                while matrix(floor(next_row), floor(next_col)) == 0
                    next_row = next_row + delta_row;
                    next_col = next_col + delta_col;
                    if (next_row < 1) || (next_row > rows) || (next_col < 1) || (next_col > cols)
                        if (next_row < 1)
                            next_row = 1;
                        elseif (next_row > rows)
                            next_row = rows;
                        end
                        if (next_col < 1)
                            next_col = 1;
                        elseif (next_col > cols)
                            next_col = cols;
                        end
                        break;
                    end
                end
                object = matrix(floor(next_row), floor(next_col));
                distance = sqrt((next_row - row) ^ 2 + (next_col - col) ^ 2);
            end
        end
        
        function [distance, object] = raycast(matrix, row, col, direction)
            deg = round(360 * mod(direction, 2 * pi) / (2 * pi));
            tangens = tan(deg2rad(deg));
            if (deg == 0)
                [distance, object] = TinUtils.raycast_hv(matrix, row, col, 0, 1);
            elseif (deg == 90)
                [distance, object] = TinUtils.raycast_hv(matrix, row, col, -1, 0);
            elseif (deg == 180)
                [distance, object] = TinUtils.raycast_hv(matrix, row, col, 0, -1);
            elseif (deg == 270)
                [distance, object] = TinUtils.raycast_hv(matrix, row, col, 1, 0);
            else
                if (deg < 90)
                    delta_col = 0.501;
                    delta_row = -0.501;
                elseif (deg < 180);
                    delta_col = -0.501;
                    delta_row = -0.501;
                elseif (deg < 270);
                    delta_col = -0.501;
                    delta_row = 0.501;
                else
                    delta_col = 0.501;
                    delta_row = 0.501;
                end
                [distance1, object1] = TinUtils.raycast_diag(matrix, row, col, -tangens * delta_col, delta_col);
                [distance2, object2] = TinUtils.raycast_diag(matrix, row, col, delta_row, -delta_row / tangens);
                if distance1 < distance2
                    distance = distance1;
                    object = object1;
                else
                    distance = distance2;
                    object = object2;
                end
            end
        end
    end
end