class Controller {
public:
    double motor_left, motor_right;
    
    double row, col, phi;
    
    bool rotating = false;
    
    void update_sensors(double* proximity, double* ir);
};

extern void update_sensors(Controller* controller, double* proximity, double* ir);