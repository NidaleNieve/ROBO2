using namespace vex;

// Brain
extern brain Brain;

//vision myndavélin
using signature = vision::signature;
extern signature Vision5__BLUEBOX;
extern signature Vision5__GREENBOX;
extern signature Vision5__REDBOX;
extern signature Vision5__SIG_4;
extern signature Vision5__SIG_5;
extern signature Vision5__SIG_6;
extern signature Vision5__SIG_7;
extern vision Vision5;

// VEXcode devices
extern motor LeftDriveSmart;
extern motor RightDriveSmart;
extern inertial DrivetrainInertial;
extern smartdrive Drivetrain;

extern sonar RangeFinderE;
extern bumper BumperH;

extern controller Controller1;
extern motor Motor8;

/**
 * Used to initialize code/tasks/devices added using tools in VEXcode Pro.
 *
 * This should be called at the start of your int main function.
 */
void vexcodeInit(void);