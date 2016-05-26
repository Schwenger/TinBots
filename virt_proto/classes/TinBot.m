classdef TinBot < handle
    properties
        map, index, row, col;
        orientation = 0;
    end
    
    methods
        function obj = TinBot(map, index, row, col)
            obj.map = map;
            obj.index = index;
            obj.row = row;
            obj.col = col;
        end
        
        function rotate(obj, delta)
            obj.orientation = mod(obj.orientation + delta, 2 * pi);
        end
        
        function move(obj, row, col)
            if (obj.map.get(row, col) ~= 0)
                error('unable to move bot to occupied area');
            end
            obj.map.set(obj.row, obj.col, 0);
            obj.map.set(row, col, 99 + obj.index);
            obj.row = row;
            obj.col = col;
        end
        
        function forward(obj, delta)
            target_row = round(obj.row - delta * obj.map.resolution * sin(obj.orientation));
            target_col = round(obj.col + delta * obj.map.resolution * cos(obj.orientation));
            if (target_row < obj.row)
                step_row = -1;
            else
                step_row = 1;
            end
            if (target_col < obj.col)
                step_col = -1;
            else
                step_col = 1;
            end
            delta_row = abs(target_row - obj.row);
            delta_col = abs(target_col - obj.col);
            next_row = obj.row;
            next_col = obj.col;
            while (delta_row ~= 0 || delta_col ~= 0)           
                if (delta_row > delta_col)
                    next_row = next_row + step_row;
                    delta_row = delta_row - 1;
                else
                    next_col = next_col + step_col;
                    delta_col = delta_col - 1;
                end
                if (obj.map.get(next_row, next_col) ~= 0)
                   	break;
                else
                    obj.move(next_row, next_col);
                end
            end
        end
        
        function backward(obj, delta)
            obj.rotate(pi);
            obj.forward(delta);
            obj.rotate(-pi);
        end           
    end
end