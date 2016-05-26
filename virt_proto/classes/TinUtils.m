classdef TinUtils < handle
    methods (Static)
        function line = readline(file_handle)
            coder.extrinsic('fgets');
            line = fgets(file_handle);
        end
        
        function x = get_bot_x(map, index)
            x = map.bots(index).col;
        end
        
        function bot_forward(map, index, delta)
            map.bots(index).forward(delta);
        end
    end
end