#ifndef main_h
#define main_h
//============INCLUDE_begin=======================
#include <list>
#include <Arduino.h>
#include "SPISlave.h"
#include "painlessMesh.h"
#include "ArduinoJSON.h"

#include "uart_callback.h"
// #include "fifo_string.h"
#include "fifo.h"

using namespace std;
//============INCLUDE_end=======================

//===============MACRO_begin========================
#define MESH_PREFIX "Mesh_username"
#define MESH_PASSWORD "mesh_password"
#define MESH_PORT 5555
#define BUTTON 5
#define FIFO_SIZE 10
#define DEBUGGING true
//===============MACRO_end========================

//=======Object_define_begin=======================
Scheduler userScheduler;
painlessMesh mesh;

Fifo_string fifo;
//=======Object_define_end=======================
//========Global_variables_define_begin==========
int buttonState = 0;
bool oldState = 0;
uint8_t ledState = 0;

list<uint32_t> currentNodeList;
vector<uint32_t> currentNodeVector;
// Task taskSendmsg( TASK_SECOND * 1 , TASK_FOREVER, &sendmsg );

struct Device
{
  uint32_t node_id;
  char name[10];
};
Device device[]{
    {2740445644, "switch"},
    {2740444166, "light_0"},
    {2740444166, "light_1"},
    {419486320, "rfid"},
    {419486320, "lock"},
    {1305485767, "temp_sen"},
    {1305485767, "humi_sen"},
    {1305485767, "hic_sen"},
    {1305485767, "gas_sen"},
};

uint32_t nodeid[] = {1305485767, 2740445644, 2740444145, 419486320, 1305484227, 2740444166};

int receive_remain = 0;
uint8_t Is_SPI_comm = 0; // Is it communicating with FB
uint8_t rx_mess[1024] = {0};
uint8_t tx_mess[1024] = {0};
uint8_t rx_package_n = 0;
uint8_t rx_package_i = 0;
uint8_t tx_package_n = 0;
uint8_t tx_package_i = 0;
uint8_t tx_package[32];
uint8_t data_size = 29;
uint32_t tx_timeout;
// String rx_message;

//========Global_variables_define_begin==========
enum SPI_STT : uint32_t
{
  STT_IDLE = 0x00000000,
  STT_OK = 0x00000001,
  STT_ERR = 0x0000000F,
};
#endif