using namespace vex;

extern brain Brain;

// VEXcode devices
extern line LineTracker1;
extern line LineTracker2;
extern line LineTracker3;

extern motor LeftMotor;
extern motor RightMotor;

extern controller Controller1;
extern motor Motor8;


/**
 * Used to initialize code/tasks/devices added using tools in VEXcode Pro.
 *
 * This should be called at the start of your int main function.
 */
void vexcodeInit(void);