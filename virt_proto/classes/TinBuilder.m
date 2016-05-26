classdef TinBuilder
    methods (Static)
        function build(filename, resolution)
            rt = sfroot;
            
            name = sprintf('tin_model_%i', randi(99999999));
            
            new_system(name);
            open_system(name);
            
            map = TinMap(filename, resolution);
            matrix = map.matrix;
                        
            add_block('built-in/Constant', [name, '/Map'], 'Position', [0 0 50 50], 'Value', mat2str(matrix));
            add_block('dspsnks4/Matrix Viewer', [name, '/Viewer'], 'Position', [300 0 350 50], 'YMax', '3', 'YMin', '0');
            
            add_block('sflib/Chart', [name, '/Environment'], 'Position', [100 -50 250 100]);
            
            model = rt.find('-isa', 'Simulink.BlockDiagram', '-and', 'Name', name);
            env = model.find('-isa','Stateflow.Chart', '-and', 'Name', 'Environment');
            
            env_map_init = Stateflow.Data(env);
            env_map_init.Scope = 'INPUT_DATA';
            env_map_init.Name = 'map_init';
            
            env_map_out = Stateflow.Data(env);
            env_map_out.Scope = 'OUTPUT_DATA';
            env_map_out.Name = 'map_out';
            
            env_map = Stateflow.Data(env);
            env_map.Scope = 'LOCAL_DATA';
            env_map.Name = 'map';           
            
            add_line(name, 'Map/1', 'Environment/1');
            add_line(name, 'Environment/1', 'Viewer/1');
            
            start_state = Stateflow.State(env);
            start_state.LabelString = ['Start', 10, 'entry:', 10, 'map = map_init;', 10, 'map_out = map;'];
            
            for index = 1:length(map.bots)
                add_block('sflib/Chart', [name, sprintf('/TinBot%i', index)], 'Position', [500 index*100-100 800 index*100-30]);
                
                bot = model.find('-isa','Stateflow.Chart', '-and', 'Name', sprintf('TinBot%i', index));
                
                motor_left = Stateflow.Data(bot);
                motor_left.Scope = 'OUTPUT_DATA';
                motor_left.Name = 'motor_left';
                
                motor_right = Stateflow.Data(bot);
                motor_right.Scope = 'OUTPUT_DATA';
                motor_right.Name = 'motor_right';
                
                leds = Stateflow.Data(bot);
                leds.Scope = 'OUTPUT_DATA';
                leds.Name = 'leds';
                
                lps_position = Stateflow.Data(bot);
                lps_position.Scope = 'INPUT_DATA';
                lps_position.Name = 'lps_position';
                
                lps_phi = Stateflow.Data(bot);
                lps_phi.Scope = 'INPUT_DATA';
                lps_phi.Name = 'lps_phi';
                
                proximity = Stateflow.Data(bot);
                proximity.Scope = 'INPUT_DATA';
                proximity.Name = 'proximity';
            end
            
                
            
            
            
            
            
            
            
            %local_map = Stateflow.Data(map_chart);
            %local_map.Scope = 'LOCAL_DATA';
            %local_map.Name = 'map';
            
            
            
            %start_state = Stateflow.State(map_chart);
            %start_state.LabelString = ['Start', 10, 'entry:', 10, 'coder.extrinsic(''TinMap'');' 10, sprintf('map = TinMap(''%s'', %i)', filename, resolution), 10];
            
            %init_trans = Stateflow.Transition(map_chart);
            %init_trans.Destination = start_state;
            %init_trans.DestinationOClock = 0;
            %xsource = start_state.Position(1) + start_state.Position(3) / 2;
            %ysource = start_state.Position(2) - 30;
            %init_trans.SourceEndPoint = [xsource ysource];
            %init_trans.MidPoint = [xsource ysource + 15];
            
            
            %disp(model);
            %disp(map_chart);
        end
    end
end