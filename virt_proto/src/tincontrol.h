class Controller {
public:
    double motor_left, motor_right;
    void update_sensors(double* proximity, double* ir);
};

extern Controller* controller;

extern Controller* create_controller();
extern void delete_controller(Controller* controller);

extern double get_motor_left(Controller* controller);
extern double get_motor_right(Controller* controller);

extern void update_sensors(Controller* controller, double* proximity, double* ir);