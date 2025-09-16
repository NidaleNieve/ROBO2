#include "vex.h"

using namespace vex;

// Constants
const int STOP_DISTANCE = 400; // Stop distance in mm (40 cm)
const int CENTER_FOV = 158;    // Center of the field of view
const int OFFSET_X = 15;       // Offset for turning

int main() {
  // Initializing Robot Configuration. DO NOT REMOVE!
  vexcodeInit();

  while (true) {
    Vision5.takeSnapshot(Vision5__REDBOX); // Look for the red object
    double distance = RangeFinder.distance(mm); // Measure distance in mm

    Brain.Screen.clearScreen();
    Brain.Screen.setCursor(1, 1);

    if (Vision5.largestObject.exists) {
      // Red object detected
      int objectCenterX = Vision5.largestObject.centerX;

      if (distance > 0 && distance < STOP_DISTANCE) {
        // Object is closer than 40 cm, back up
        RightMotor.spin(reverse);
        LeftMotor.spin(reverse);
        Brain.Screen.print("Status: Backing Up");
      } else if (distance > STOP_DISTANCE) {
        // Object is farther than 40 cm, move forward
        RightMotor.spin(forward);
        LeftMotor.spin(forward);
        Brain.Screen.print("Status: Moving Forward");
      } else {
        // Object is exactly at 40 cm, stop
        RightMotor.stop(brakeType::brake);
        LeftMotor.stop(brakeType::brake);
        Brain.Screen.print("Status: Stopped (At 40 cm)");
      }

      // Adjust direction if the object is not centered
      if (objectCenterX > CENTER_FOV + OFFSET_X) {
        // Object is to the right, turn right slightly
        RightMotor.spin(reverse);
        LeftMotor.spin(forward);
        Brain.Screen.setCursor(2, 1);
        Brain.Screen.print("Adjusting: Turning Right");
      } else if (objectCenterX < CENTER_FOV - OFFSET_X) {
        // Object is to the left, turn left slightly
        RightMotor.spin(forward);
        LeftMotor.spin(reverse);
        Brain.Screen.setCursor(2, 1);
        Brain.Screen.print("Adjusting: Turning Left");
      }
    } else {
      // No red object detected, stop the robot
      RightMotor.stop(brakeType::brake);
      LeftMotor.stop(brakeType::brake);
      Brain.Screen.print("Status: Stopped (No Red Detected)");
    }

    task::sleep(20); // Small delay to prevent CPU overload
  }
}