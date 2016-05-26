classdef TinMap < handle
    properties
        resolution = 10;
        width = -1;
        height = 0;
        matrix = [];
        bots = [];
        victims = [];
    end
        
    methods
        function obj = TinMap(filename, resolution)
            obj.resolution = resolution;
            map_file = fopen(filename, 'r');
            line = TinUtils.readline(map_file);
            while (line ~= -1)
                line = strtrim(line);
                if (obj.width == -1)
                    obj.width = length(line) * resolution;
                end
                for i = 1:resolution
                    obj.height = obj.height + 1;
                    if (length(line) > obj.width)
                        error('line %i is too long', obj.height);
                    end
                    row = zeros(1, obj.width);
                    index = 1;
                    while index <= length(line)
                        c = line(index);                        
                        switch c
                            case '#'
                                row((index - 1) * resolution + 1:index * resolution) = 1;
                            case 'T'
                                if i == round(resolution / 2)
                                    col = round((index - 1) * resolution + resolution / 2);
                                    %row(col) = 2; %length(obj.bots) + 100;
                                    obj.bots = [obj.bots; TinBot(obj, length(obj.bots) + 1, obj.height, col)];
                                end
                            case 'V'
                                if i == round(resolution / 2)
                                    col = round((index - 1) * resolution + resolution / 2);
                                    row(col) = 2; %length(obj.victims) + 200;
                                    obj.victims = [obj.victims; TinVictim(obj, length(obj.victims) + 1, obj.height, col)];
                                end
                        end
                        index = index + 1;
                    end
                    obj.matrix = [obj.matrix; row];
                end                
                line = TinUtils.readline(map_file);
            end
            fclose(map_file);
        end
        
        function value = get(obj, row, col)
            value = obj.matrix(row, col);
        end
        
        function set(obj, row, col, value)
            obj.matrix(row, col) = value;
        end
    end
end