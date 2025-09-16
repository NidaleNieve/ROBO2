using namespace vex;

// Brain
extern brain Brain;

// Vision Sensor
using signature = vision::signature;
extern signature Vision5__BLUEBOX;
extern signature Vision5__GREENBOX;
extern signature Vision5__REDBOX;
extern signature Vision5__SIG_4;
extern signature Vision5__SIG_5;
extern signature Vision5__SIG_6;
extern signature Vision5__SIG_7;
extern vision Vision5;

// Motors
extern motor LeftMotor;
extern motor RightMotor;
extern sonar RangeFinder;

// Distance Sensor (if needed)
extern distance DistanceSensor;

// Gyroscope (if needed)
extern gyro GyroSensor;

/**
 * Used to initialize code/tasks/devices added using tools in VEXcode Pro.
 *
 * This should be called at the start of your int main function.
 */
void vexcodeInit(void);