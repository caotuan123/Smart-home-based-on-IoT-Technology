#ifndef MAIN_H
#define MAIN_H


#if defined(ESP32)
#include <WiFi.h>
#include <FirebaseESP32.h>

#endif

// Provide the token generation process info.
#include <addons/TokenHelper.h>

// Provide the RTDB payload printing info and other helper functions.
#include <addons/RTDBHelper.h>

#include <SPI.h>

#include "fifo_string.h"

#include "ArduinoJson.h"
/* 1. Define the WiFi credentials */
#define WIFI_SSID "projekt"
#define WIFI_PASSWORD "13579011"
// For the following credentials, see examples/Authentications/SignInAsUser/EmailPassword/EmailPassword.ino

/* 2. Define the API Key */
#define API_KEY ""

/* 3. Define the RTDB URL */
#define DATABASE_URL "" //<databaseName>.firebaseio.com or <databaseName>.<region>.firebasedatabase.app

/* 4. Define the user Email and password that alreadey registerd or added in your project */
#define USER_EMAIL ""
#define USER_PASSWORD ""

#define SPI_SS_PIN 15 // select pin for SPI

//========ESPMaster class begin===========================
#define DEBUGGING true
enum SPI_STT : uint32_t {
    STT_IDEL      = 0x00000000,
		STT_OK				= 0x00000001,	
		STT_ERR				= 0x0000000F,
    
	};
class ESPMaster
{
private:
  uint8_t _ss_pin;

public:
	
  ESPMaster(uint8_t pin)
      : _ss_pin(pin) {}
  void begin()
  {
    pinMode(_ss_pin, OUTPUT);
    digitalWrite(_ss_pin, HIGH);
  }

  uint32_t readStatus()
  {
    digitalWrite(_ss_pin, LOW);
    SPI.transfer(0x04);
    uint32_t status = (SPI.transfer(0) | ((uint32_t)(SPI.transfer(0)) << 8) | ((uint32_t)(SPI.transfer(0)) << 16) | ((uint32_t)(SPI.transfer(0)) << 24));
    digitalWrite(_ss_pin, HIGH);
    return status;
  }

  void writeStatus(uint32_t status)
  {
    digitalWrite(_ss_pin, LOW);
    SPI.transfer(0x01);
    SPI.transfer(status & 0xFF);
    SPI.transfer((status >> 8) & 0xFF);
    SPI.transfer((status >> 16) & 0xFF);
    SPI.transfer((status >> 24) & 0xFF);
    digitalWrite(_ss_pin, HIGH);
  }

  void readData(uint8_t *data)
  {
    digitalWrite(_ss_pin, LOW);
    SPI.transfer(0x03);
    SPI.transfer(0x00);
    for (uint8_t i = 0; i < 32; i++)
    {
      data[i] = SPI.transfer(0);
      
    }
    digitalWrite(_ss_pin, HIGH);
  }
  void writeData(uint8_t *data, size_t len)
  {
    uint8_t i = 0;
    digitalWrite(_ss_pin, LOW);
    SPI.transfer(0x02);
    SPI.transfer(0x00);
    while (len-- && i < 32)
    {
      SPI.transfer(data[i++]);
    }
    while (i++ < 32)
    {
      SPI.transfer(0);
    }
    digitalWrite(_ss_pin, HIGH);
  }

  String readData()
  {
    char data[33];
    data[32] = 0;
    readData((uint8_t *)data);
    return String(data);
  }

 void writeData(String mess){
    uint16_t mess_len = mess.length();
     if(mess_len>1024)
    {
       (DEBUGGING)?Serial.println("This messager is too large!"):0; 
       return;
    }
    uint8_t package[32];
    const uint8_t data_size=29;
    uint8_t BCC;
    uint8_t single_data;
    uint8_t package_n=(mess_len/data_size)+ (uint8_t)((mess_len%data_size)?1:0);
    
  const uint32_t timeout = (uint32_t)millis() + 5;// create timer for timeout (just in case) 
	int i=0;
	while((millis()<=timeout)&&(i<package_n)){ // set timeout to 5 ms 
		  package[0]=package_n;
      package[1]=i;
      uint8_t data[data_size];
      BCC=0;     
      for(int j=0;j<data_size;j++)
      {
        single_data=mess.charAt(j+i*data_size);
        BCC^=single_data;
        package[3+j]=single_data;
      }
      package[2]=BCC;
      writeData((uint8_t *)package,32);
      delayMicroseconds(30);
      if(readStatus()==STT_ERR){
        i=0;
      }
      i++;
		  
	}
 }
};

//========ESPMaster class end===========================

//=========Object define begin============================

Fifo_string fifo = Fifo_string();
ESPMaster esp(SPI_SS_PIN);
// Define Firebase Data object
FirebaseData fbdo;
FirebaseData stream;
FirebaseAuth auth;
FirebaseConfig config;
//=========Object define end============================

//=========Global variables define begin================

volatile bool Is_new_mess_SPI=false;
String message;
unsigned long sendDataPrevMillis = 0;

String project_path="/test";

String streamPath = "/is_new_command";
String path_cmd_list="/list_command";
String path_list_device="/list_device";
volatile bool dataChanged = false;
struct Device
{
  String name;
  int index;
};

struct Device list_device[]={{(String)"light",0},{(String)"lock",1},{(String)"rfid",2}};




//=========Global variables define begin================
unsigned long time_read_RTDB=0;
#endif
