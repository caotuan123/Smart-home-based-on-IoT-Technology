#ifndef MAIN_H
#define MAIN_H
//==========Include_begin===================
#include "painlessMesh.h"
#include "ArduinoJSON.h"
#include <SPI.h>
#include "mfrc522_custom.h"
#include <MFRC522.h>
#include "fifo_string.h"
#include "lock.h"
#include "mesh.h"

// #include <TaskScheduler.h>
//=========Include_end======================
//=========Define_begin=====================
#define MESH_PREFIX "Mesh_username"
#define MESH_PASSWORD "mesh_password"
#define MESH_PORT 5555
#define MESH_HOST_ID 848061016
#define SS_PIN 4
#define RST_PIN 5

#define GREEN_LED 0
#define RED_LED 15
#define PERIOD 150
#define DURATION 1000
//=========Define_end=======================
struct Node
{
  uint32_t id;
  String node_name;
  int a;
  int b;
  int c;
};
//==========Global_variables_begin==========
struct Node node[6];
uint32_t nodeid[] = {1305485767, 2740445644, 2740444145, 419486320, 1305484227, 2740444166};
volatile bool greenLedState = false;
volatile bool redLedState = false;
//=========Global_vaviables_end=============

//=========function declaration begin=======
void redBlink();
void greenBlink();
//=========function declaration end=========
//=========Object_begin=====================
MFRC522_CUSTOM rfid(SS_PIN, RST_PIN, 2); // Instance of the class

MFRC522::MIFARE_Key key;

// Init array that will store new NUID
byte nuidPICC[4];

Scheduler userScheduler;
painlessMesh mesh;

Lock lock(2, 3);
FifoString fifo;
// Scheduler ts;
Task rBlink(PERIOD *TASK_MILLISECOND, DURATION / PERIOD, &redBlink, &userScheduler, false);
Task gBlink(PERIOD *TASK_MILLISECOND, DURATION / PERIOD, &greenBlink, &userScheduler, false);
//==========Object_end======================

//==========Function========================
void redBlink()
{
  if (rBlink.isFirstIteration())
  {
    redLedState = false;
  }

  if (redLedState)
  {
    digitalWrite(RED_LED, LOW);
    redLedState = false;
  }
  else
  {
    digitalWrite(RED_LED, HIGH);
    redLedState = true;
  }
  if (rBlink.isLastIteration())
  {
    digitalWrite(RED_LED, LOW);
  }
}

void greenBlink()
{
  if (gBlink.isFirstIteration())
  {
    greenLedState = false;
  }

  if (greenLedState)
  {
    digitalWrite(GREEN_LED, LOW);
    greenLedState = false;
  }
  else
  {
    digitalWrite(GREEN_LED, HIGH);
    greenLedState = true;
  }
  if (gBlink.isLastIteration())
  {
    digitalWrite(GREEN_LED, LOW);
  }
}

#endif