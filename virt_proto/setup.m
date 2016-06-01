function setup()
    addpath('classes');
    addpath('library');
    addpath('software');
    addpath('tests');
       
    disp(fileread('README.txt'));
    
    open('library/tinbot.slx');
end