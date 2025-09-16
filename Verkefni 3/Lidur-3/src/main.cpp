#include "vex.h"

using namespace vex;

// define variable for remote controller enable/disable
bool RemoteControlCodeEnabled = true;

#pragma endregion VEXcode Generated Robot Configuration

bool EmergencyStop;
int Direction;

//Emergency Stop triggers
void onevent_Controller1ButtonX_pressed_0() {
  EmergencyStop = true;
}
void onevent_BumperH_pressed_0() {
  EmergencyStop = true;
}

void displayFunction() {
  Brain.Screen.clearLine(1);
  Brain.Screen.setCursor(1, 1);

  //prenta direction
  if (Direction == 1) {
    Brain.Screen.print("Direction: Forward ");
  } else if (Direction == -1) {
    Brain.Screen.print("Direction: Backward");
  } else if (Direction == 2) {
    Brain.Screen.print("Direction: Right");
  } else if (Direction == 3) {
    Brain.Screen.print("Direction: Left");
  } else {
    Brain.Screen.print("Stopped ");
  }
}

int displayThread() {
  while (true) {
    displayFunction();
    wait(100, msec);
  }
  return 0;
}

// Constants
const int STOP_DISTANCE = 400; // Stop distance in mm (40 cm)
const int CENTER_FOV = 158;    // Center of the field of view
const int OFFSET_X = 15;       // Offset for turning

int main() {
  // Initializing Robot Configuration. DO NOT REMOVE!
  vexcodeInit();

  // starta event handlers, fyrir emergency stop 
  Controller1.ButtonX.pressed(onevent_Controller1ButtonX_pressed_0);
  BumperH.pressed(onevent_BumperH_pressed_0);
  
  thread displaythread = thread(displayThread);

  wait(15, msec);

  //aðal forritið
  EmergencyStop = false;
  Drivetrain.setTurnVelocity(20.0, percent);
  Drivetrain.setDriveVelocity(30.0, percent);
  // Færi Arm upp þannig hann sé ekki fyrir
  Motor8.setVelocity(20.0, percent);
  Motor8.spinToPosition(120.0, degrees, true);

  Direction = 0;
  //keyri bara ef að emergency stop er ekki active
  while ((!EmergencyStop)) {
    Vision5.takeSnapshot(Vision5__REDBOX); // Look for the red object
    double distance = RangeFinderE.distance(mm); // Measure distance in mm

    if (Vision5.largestObject.exists) {
      // Red object detected
      int objectCenterX = Vision5.largestObject.centerX;

      if (distance > 0 && distance < STOP_DISTANCE) {
        //keyri afturábak
        Drivetrain.drive(reverse);
        Direction = -1;
      } else if (distance > STOP_DISTANCE) {
        //keyri áfram
        Drivetrain.drive(forward);
        Direction = 1;
      } else {
        Drivetrain.stop();
        Direction = 0;
      }

      // Adjust direction if the object is not centered
      if (objectCenterX > CENTER_FOV + OFFSET_X) {
        //færi til hægri
        Drivetrain.turn(right);
        Direction = 2;
      } else if (objectCenterX < CENTER_FOV - OFFSET_X) {
        //færi til vinstri
        Drivetrain.turn(left);
        Direction = 3;
      }
    } else {
      Drivetrain.stop();
    }
    wait(5, msec);
  }
  //stoppa alla mótora og forritið
  Motor8.stop();
  Drivetrain.stop();
  Brain.programStop();
  return 0;

}