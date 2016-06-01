function setup()
    addpath('classes');
    addpath('library');
    addpath('software');
    
    newData1 = load('proto.mat');
    % Create new variables in the base workspace from those fields.
    vars = fieldnames(newData1);
    for i = 1:length(vars)
        assignin('base', vars{i}, newData1.(vars{i}));
    end