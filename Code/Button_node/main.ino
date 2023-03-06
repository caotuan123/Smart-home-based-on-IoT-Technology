#include "painlessMesh.h"
#include "ArduinoJSON.h"

#include "control_light.h"
#include "ArduinoJson.h"
#define MESH_PREFIX "Mesh_username"
#define MESH_PASSWORD "mesh_password"
#define MESH_PORT 5555
#define MESH_HOST_ID 848061016
#define BUTTON 5
#define LIGHT_ID 2740444166

Scheduler userScheduler;
painlessMesh mesh;

ControlLight light1(2740444166);

int buttonState = 0;
bool oldState = 0;
uint8_t ledState = 0;
int sendingNode = 0;

void sendmsg(String cmd, uint32_t node_id)
{

  int test = mesh.sendSingle(node_id, cmd);

  if (test)
    Serial.printf("CMD: %s,Node:%d send success", cmd, node_id);
  else
    Serial.printf("CMD: %s,Node:%d cannot send", cmd, node_id);
}

void receivedCallback(uint32_t from, String &msg)
{
  Serial.printf("Received from %u msg=%s\n", from, msg.c_str());
  String json;
  DynamicJsonDocument doc(1024);
  json = msg.c_str();
  DeserializationError error = deserializeJson(doc, json);
  if (error)
  {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
  }
}

void newConnectionCallback(uint32_t nodeId)
{
  Serial.printf("New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback()
{
  Serial.printf("Changed connections\n");
}

void nodeTimeAdjustedCallback(int32_t offset)
{
  Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(), offset);
}

void setup()
{

  Serial.begin(115200);

  pinMode(BUTTON, INPUT_PULLUP);

  mesh.setDebugMsgTypes(ERROR | STARTUP);

  mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT);
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
}

void loop()
{
  mesh.update();

  oldState = buttonState;
  buttonState = digitalRead(BUTTON);
  if (buttonState && (oldState != buttonState))
  {
    DynamicJsonDocument doc(1024);
    String mess;
    doc["type"] = 1;
    doc["device"] = "light_0";
    doc["cmd"] = "TOGGLE";
    serializeJson(doc, Serial);
    serializeJson(doc, mess);
    bool tmp = mesh.sendSingle(LIGHT_ID, mess);
    (tmp) ? Serial.println("OK") : Serial.println("Can not control the light");
  }
}
