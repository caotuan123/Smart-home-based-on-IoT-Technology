#include "main.h"

void sendmsg(String cmd, uint32_t node_id)
{
  mesh.sendSingle(node_id, cmd);
}

void receivedCallback(uint32_t from, String &msg)
{
  Serial.printf("Received from %u msg=%s\n", from, msg.c_str());
  fifo.push_element(msg);
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
  // Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(), offset);
}

void cmd_handler(String json_data)
{
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, json_data);
  JsonObject obj = doc.as<JsonObject>();
  String cmd = obj["cmd"];
  String device = obj["device"];
  // int type = obj["type"];
  doc.~BasicJsonDocument(); // let doc free
  if (device == (String) "lock")
  {
    lock.excute(cmd);
  }
  else if (device == (String) "rfid")
  {
    rfid.execute(cmd);
  }
}
void setup()
{

  Serial.begin(115200);
  Serial.setDebugOutput(true);
  // For RFID---------------------
  SPI.begin();     // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522
  // For Mesh----------------------
  mesh.setDebugMsgTypes(ERROR | STARTUP);
  mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT);
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
  mesh.setContainsRoot();
  //------------------------------

  for (byte i = 0; i < 6; i++)
  {
    key.keyByte[i] = 0xFF;
  }
}

void loop()
{
  mesh.update();
  if (!fifo.isEmpty())
  {
    String cmd_tmp = fifo.get_element();
    cmd_handler(cmd_tmp);
  }

  if (lock.IsLockOpen() && (lock.getTimeOut() <= millis()))
  {
    lock.LockClose();
  }
  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if (!rfid.PICC_IsNewCardPresent())
    return;
  // Verify if the NUID has been readed
  if (!rfid.PICC_ReadCardSerial())
    return;

  //=================================
  uint64_t tmp_uid = rfid.transformUid(rfid.uid.uidByte, rfid.uid.size);
  if (rfid.searchUser(tmp_uid))
  {
    // Serial.println(F("Welcom !!!!!!!!"));
    gBlink.restart();
    lock.LockOpen();
  }
  else
  {
    rBlink.restart();
    // Serial.println(F("WHO ARE YOU??????????"));
  }

  // Halt PICC
  rfid.PICC_HaltA();

  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
}
