/*----------------------------------------------------------------------------------*/
/*                                                                                  */
/*    Module:             main.cpp                                                  */
/*    Author:             VEX                                                       */
/*    Created:            Wed Jun 09 2021                                           */
/*    Description:        Drive to Location (Known Starting Position)               */
/*                        This example will show how to use a GPS Sensor to         */
/*                        navigate a V5 Moby Hero Bot to the center of a field      */
/*                        by driving along the X-axis then the Y-axis               */
/*                                                                                  */
/*    Starting Position:  Bottom Right Corner - Facing West                         */
/*                                                                                  */
/*----------------------------------------------------------------------------------*/

// ---- START VEXCODE CONFIGURED DEVICES ----
// Robot Configuration:
// [Name]               [Type]        [Port(s)]
// Drivetrain           drivetrain    1, 10, 3        
// ForkMotorGroup       motor_group   2, 9            
// Rotation4            rotation      4               
// GPS9                 gps           8               
// DistanceLeft         distance      12              
// DistanceRight        distance      20              
// Optical19            optical       19              
// BumperA              bumper        A               
// ---- END VEXCODE CONFIGURED DEVICES ----

#include "vex.h"

using namespace vex;


bool EmergencyStop;

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

  Brain.Screen.print("X:%.2f Y:%.2f",
                      GPS9.xPosition(mm),
                      GPS9.yPosition(mm)
                    );
}

int displayThread() {
  while (true) {
    displayFunction();
    wait(100, msec);
  }
  return 0;
}

const int STOP_DISTANCE = 100; // Stop distance in mm (10 cm)
const int CENTER_FOV = 158;    // Center of the field of view
const int OFFSET_X = 25;       // Offset for turning
double driveVelocity = 30.0;

enum TargetColor { RED, BLUE, GREEN, BUINN };
TargetColor currentTarget = RED;
bool boxHandled = false;

int main() {
  // Calibrate the GPS Sensor before starting
  GPS9.calibrate();
  while (GPS9.isCalibrating()) { task::sleep(50); }

  // Set the approximate starting position of the robot
  // This helps the GPS Sensor know its starting position
  // if it is too close to the field walls to get an accurate initial reading
  GPS9.setLocation(56, -45, inches, 270, degrees);
  
  EmergencyStop = false;
  Controller1.ButtonX.pressed(onevent_Controller1ButtonX_pressed_0);
  BumperH.pressed(onevent_BumperH_pressed_0);
  thread displaythread = thread(displayThread);

  wait(15, msec);

  //aðal forritið
  Drivetrain.setTurnVelocity(8.0, percent);
  Drivetrain.setDriveVelocity(driveVelocity, percent);
  // Færi Arm upp þannig hann sé ekki fyrir
  ArmMotor.setVelocity(20.0, percent);
  ArmMotor.spinToPosition(120.0, degrees, true);

  //stoppar forritið ef að currentTarget er búið
  while ((!EmergencyStop) && currentTarget != BUINN) {
    Vision5.takeSnapshot(Vision5__REDBOX);
    Vision5.takeSnapshot(Vision5__GREENBOX);
    Vision5.takeSnapshot(Vision5__BLUEBOX);
    double distance = RangeFinderE.distance(mm); // Measure distance in mm
    
    if (distance > (STOP_DISTANCE - 50) && distance < (STOP_DISTANCE + 50)) {
      Drivetrain.stop();
    
    // Bakkar ef kassi er of nær, +- 5 cm
    } else if (distance <= (STOP_DISTANCE - 50)) {
      Drivetrain.setDriveVelocity(driveVelocity / 3, percent);
      Drivetrain.drive(reverse);
    } else if (Vision5.largestObject.exists) {
      //set hraðann aftur á 30
      Drivetrain.setDriveVelocity(driveVelocity, percent);
      // detected
      int objectCenterX = Vision5.largestObject.centerX;

      // Ef ekki centered, snýr
      if (objectCenterX > CENTER_FOV + OFFSET_X) {
        //beygir til hægri
        Drivetrain.stop();
        Drivetrain.turn(right);
      } else if (objectCenterX < CENTER_FOV - OFFSET_X) {
        //beygir til vinstri
        Drivetrain.stop();
        Drivetrain.turn(left);
      } else {
        //beygir áfram
        Drivetrain.drive(forward);
      }
    } else {
      //ef ekkert fundið, stoppar
      Drivetrain.stop();
    }
    wait(5, msec);
  }

  //stoppa alla mótora og forritið
  ArmMotor.stop();
  Drivetrain.stop();
  Brain.programStop();
  return 0;


  /*
  Drivetrain.drive(forward);

  // Keep driving until the GPS's xPosition passes 0 (horizontal center)
  while (!(GPS9.xPosition(mm) < 0)) {
    wait(0.1, seconds);
  }
  Drivetrain.stop();

  Drivetrain.turnToHeading(90, degrees, true);
  Drivetrain.drive(forward);

  // Keep driving until the GPS's yPosition passes 0 (vertical center)
  while (!(GPS9.yPosition(mm) > 0)) {
    wait(0.1, seconds);
  }
  Drivetrain.stop();
  */
}