/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       main.cpp                                                  */
/*    Author:       VEX                                                       */
/*    Created:      Thu Sep 26 2019                                           */
/*    Description:  V5 project                                                */
/*                                                                            */
/*   This program will have the robot move left when it senses higher         */
/*   reflectivity than the threshold (Light maximum reflectivity +            */
/*   Dark maximum reflectivity / 2) and will move to the right when it        */
/*   senses less reflectivity than the threshold.                             */
/*                                                                            */
/*   IMPORTANT: In order for this program to work correctly, the Line         */
/*   Tracker needs to be mounted facing down towards the ground, close        */
/*   to the ground.                                                           */
/*                                                                            */
/*----------------------------------------------------------------------------*/

// ---- START VEXCODE CONFIGURED DEVICES ----
// Robot Configuration:
// [Name]               [Type]        [Port(s)]
// LineTrackerA         line          A
// LeftMotor            motor         1
// RightMotor           motor         10
// ---- END VEXCODE CONFIGURED DEVICES ----

#include "vex.h"

using namespace vex;

bool EmergencyStop;
int margin;
int lastDirection;
int Direction; //1=miðju, 2=hægrimegin, 3=vinstrimegin, 0=stopped

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

  Brain.Screen.print("L:%3d C:%3d R:%3d",
                      LineTracker1.reflectivity() /*+ 3*/,
                      LineTracker2.reflectivity() /*- 5*/,
                      LineTracker3.reflectivity()
                    );


  Brain.Screen.clearLine(2);
  Brain.Screen.setCursor(2, 2);
  //prenta direction
  if (Direction == 1) {
    Brain.Screen.print("Er i Midjunni ");
  } else if (Direction == 2) {
    Brain.Screen.print("Er Haegramegin ");
  } else if (Direction == 3) {
    Brain.Screen.print("Er Vinstramegin ");
  } else if (Direction == 4) {
    Brain.Screen.print("Er Sma Vinstramegin ");
  } else if (Direction == 5) {
    Brain.Screen.print("Er Sma Haegramegin ");
  } else if (Direction == 6) {
    Brain.Screen.print("Er fyrir utan Vinstramegin ");
  } else if (Direction == 7) {
    Brain.Screen.print("Er fyrir utan Haegramegin ");
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

int main() {
  // Initializing Robot Configuration. DO NOT REMOVE!
  vexcodeInit();
  
  //emergency stop handlers
  EmergencyStop = false;
  Controller1.ButtonX.pressed(onevent_Controller1ButtonX_pressed_0);
  BumperH.pressed(onevent_BumperH_pressed_0);

  thread displaythread = thread(displayThread);

  //c 26
  //L 20 - 24
  //R 20 - 24

  //allir 94 ef að engin lína

  margin = 30;
  lastDirection = 1;

  int lastLeft = 0;
  int lastCenter = 0;
  int lastRight = 0;

  while ((!EmergencyStop)) {
    int left  = LineTracker1.reflectivity();
    int center = LineTracker2.reflectivity();
    int right = LineTracker3.reflectivity();

    //Ef að er í miðjunni
    if (center < 70 && left > 60 && right > 60) {
      //fer áfram
      LeftMotor.spin(forward, 25, percent);
      RightMotor.spin(forward, 25, percent);
      Direction = 1;
      lastDirection = 1;

      lastLeft = left;
      lastCenter = center;
      lastRight = right;
      wait(50, msec);

    //Ef að hægri skynjarinn er smá
    //} else if (left < 70 && center < 60) 
    } else if (center < 80 && left < 60 && right > 60) {
      //Beygir til vinstri
      LeftMotor.spin(forward, 25, percent);
      RightMotor.spin(forward, 21, percent);
      Direction = 4;
      lastDirection = 3;

      lastLeft = left;
      lastCenter = center;
      lastRight = right;

    //Ef að er vinstramegin er smá
    //} else if (right < 70 && center < 60) {
    } else if (center < 80 && right < 60 && left > 50) {
      //Beygir til hægri
      LeftMotor.spin(forward, 21, percent);
      RightMotor.spin(forward, 25, percent);
      Direction = 5;
      lastDirection = 2;

      lastLeft = left;
      lastCenter = center;
      lastRight = right;

    //Ef að er hægramegin er minnstur
    //} else if (left < 70) {
    } else if (center > 60 && left < 70 && right > 50) {
    //} else if (left + margin < right) {
      //Beygir til vinstri
      LeftMotor.spin(forward, 25, percent);
      RightMotor.spin(forward, 13, percent);
      Direction = 3;
      lastDirection = 3;

      lastLeft = left;
      lastCenter = center;
      lastRight = right;

    //Ef að er vinstramegin er minnstur
    //} else if (right < 70) {
    } else if (center > 60 && right < 70 && left > 60) {
      //Beygir til hægri
      LeftMotor.spin(forward, 13, percent);
      RightMotor.spin(forward, 25, percent);
      Direction = 2;
      lastDirection = 2;

      lastLeft = left;
      lastCenter = center;
      lastRight = right;

    } else {
      //outside correction
            if (lastDirection == 2) {
        // Sharp turn left
        LeftMotor.spin(forward, 0, percent);
        RightMotor.spin(forward, 25, percent);
        Direction = 6;
        wait(150, msec);
        if (LineTracker1.reflectivity() > 70 &&
            LineTracker2.reflectivity() > 70 &&
            LineTracker3.reflectivity() > 70) {
          // Gradual turn left if line not found
          LeftMotor.spin(forward, 5, percent);
          RightMotor.spin(forward, 20, percent);
        }
        // beygir til hægri ef að var vinstramegin
      } else if (lastDirection == 3) {
        // Sharp turn right
        LeftMotor.spin(forward, 25, percent);
        RightMotor.spin(forward, 0, percent);
        Direction = 7;
        wait(150, msec);
        if (LineTracker1.reflectivity() > 70 &&
            LineTracker2.reflectivity() > 70 &&
            LineTracker3.reflectivity() > 70) {
          // Gradual turn right if line not found
          LeftMotor.spin(forward, 20, percent);
          RightMotor.spin(forward, 5, percent);
        }
      } else {
        // Stoppar ef engin lína er fundin
        LeftMotor.stop();
        RightMotor.stop();
        Direction = 0;
      }
      /*
      //beygir til vinstri ef að var hægramegin
      if (lastDirection == 2) {
        LeftMotor.spin(forward, 5, percent);
        RightMotor.spin(forward, 20, percent);
        Direction = 6;
      //beygir til hægri ef að var vinstramegin
      } else if (lastDirection == 3) {
        LeftMotor.spin(forward, 20, percent);
        RightMotor.spin(forward, 5, percent);
        Direction = 7;
      } else {
        //Stoppar ef engin lína er fundin
        LeftMotor.stop();
        RightMotor.stop();
        Direction = 0;
      }
      */
     
      /*
      //stoppar ef engin lína hefur verið fundin
      if (lastRight == 0 && lastLeft == 0 && lastCenter == 0) {
        //Stoppar ef engin lína er fundin initially
        LeftMotor.stop();
        RightMotor.stop();
        Direction = 0;

      //beygir til vinstri ef að var hægramegin
      } else if (lastRight + margin < lastLeft) {
        LeftMotor.spin(forward, 7, percent);
        RightMotor.spin(forward, 30, percent);
        Direction = 2;

      //beygir til hægri ef að var vinstramegin
      } else if (lastLeft + margin < lastRight) {
        LeftMotor.spin(forward, 30, percent);
        RightMotor.spin(forward, 7, percent);
        Direction = 3;

      } else {
        //Stoppar ef engin lína er fundin
        LeftMotor.stop();
        RightMotor.stop();
        Direction = 0;
      }
      */
    }
    wait(3, msec);
  }

  LeftMotor.stop();
  RightMotor.stop();
  Brain.programStop();
  return 0;

}
