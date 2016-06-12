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
    end
end