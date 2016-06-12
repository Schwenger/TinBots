function victim_phi = determine_victim_phi(ir_start, ir_end, sensor_index)

while ir_end < ir_start
    ir_end = ir_end + 2*pi;
end

look_phi = (ir_start + ir_end) / 2;
victim_phi = look_phi + (sensor_index - 1) * pi/3;
victim_phi = mod(victim_phi, 2*pi);

end
