#include "main.h"

void receivedCallback(uint32_t from, String &msg)
{
  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
  fifo.push_element(msg);
}

void newConnectionCallback(uint32_t nodeId)
{
  Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback()
{
  Serial.printf("Changed connections\n");
}

void nodeTimeAdjustedCallback(int32_t offset)
{
  Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(), offset);
}

void cmd_handler(String json_data)
{
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, json_data);
  JsonObject obj = doc.as<JsonObject>();
  String cmd = obj["cmd"];
  String device = obj["device"];
  int type = obj["type"];
  Serial1.printf("type %d\n", type);
  if (device == light0.get_name())
  {
    light0.Excute(cmd);
    Serial.println("light_0==");
  }
  else if (device == light1.get_name())
    light1.Excute(cmd);
}

void setup()
{
  Serial.begin(115200);

  mesh.setDebugMsgTypes(ERROR | STARTUP); // set before init() so that you can see startup messages
  mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT);
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
  mesh.setContainsRoot();
}

void loop()
{
  mesh.update();
  if (!fifo.isEmpty())
  {
    cmd_handler(fifo.get_element());
  }
}
