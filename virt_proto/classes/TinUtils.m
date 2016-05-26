classdef TinUtils < handle
    methods (Static)
        function line = readline(file_handle)
            line = fgets(file_handle);
        end
    end
end