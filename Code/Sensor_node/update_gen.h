#ifndef UPDATE_GEN_H_
#define UPDATE_GEN_H_

#include "ArduinoJson.h"
// This function is responsible for creating a message
// to update the status information to the database
String update_generate(String device, String info, String value, int index);
#endif