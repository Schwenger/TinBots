function setup()
    addpath('classes');
    addpath('library2');
    addpath('software');
    addpath('tests');
       
    disp(fileread('README.txt'));
    
    open('library2/tinbot.slx');
end
