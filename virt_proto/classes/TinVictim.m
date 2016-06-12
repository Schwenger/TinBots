classdef TinVictim < handle
    properties
        map, index, row, col;
    end
    
    methods
        function obj = TinVictim(map, index, row, col)
            obj.map = map;
            obj.index = index;
            obj.row = row;
            obj.col = col;
        end
    end
end