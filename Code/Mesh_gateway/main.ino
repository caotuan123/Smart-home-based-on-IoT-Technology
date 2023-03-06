#include "main.h"

void sendmsg(String cmd, uint32_t node_id)
{
  int send = mesh.sendSingle(node_id, cmd);
  if (send)
  {
    (DEBUGGING) ? Serial1.printf("Send %s to %u success\n", cmd, node_id) : 0;
  }
  else
    (DEBUGGING) ? Serial1.printf("Cannot send %s to %lu \n", cmd, node_id) : 0;
}

void receivedCallback(uint32_t from, String &msg)
{
  (DEBUGGING) ? Serial1.printf("Received from %u msg=%s\n", from, msg.c_str()) : 0;

  fifo.push_element(msg);
}

void newConnectionCallback(uint32_t nodeId)
{
  (DEBUGGING) ? Serial1.printf("New Connection, nodeId = %u\n", nodeId) : 0;
}

void changedConnectionCallback()
{
  (DEBUGGING) ? Serial1.printf("Changed connections\n") : 0;
}

void nodeTimeAdjustedCallback(int32_t offset)
{
  (DEBUGGING) ? Serial1.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(), offset) : 0;
}

void set_package_to_SPI()
{

  tx_package[0] = tx_package_n;
  tx_package[1] = tx_package_i;
  uint8_t BCC = 0;
  for (int i = 0; i < data_size; i++)
  {
    BCC ^= tx_mess[i + tx_package_i * data_size];
    tx_package[32 - data_size + i] = tx_mess[i + tx_package_i * data_size];
  }
  tx_package[2] = BCC;
  SPISlave.setData(tx_package, 32);

  tx_package_i++;
}
void send_to_master(String data_str)
{
  // FIRST TIME
  data_str.toCharArray((char *)tx_mess, 1024);
  Serial1.printf("data_str: %s\n", data_str);
  // strcpy((char*)tx_mess,data_str);
  int mess_len = strlen((char *)tx_mess);
  if (mess_len > 1024)
  {
    return;
  }
  Is_SPI_comm = 1; // True
  tx_package_i = 0;
  tx_package_n = (mess_len / data_size) + (uint8_t)((mess_len % data_size) ? 1 : 0);
  tx_timeout = (uint32_t)millis() + 200;
  set_package_to_SPI();
  digitalWrite(1, LOW);
  delay(1);
  digitalWrite(1, HIGH);
}
void onDataCallback(uint8_t *data, size_t len) // recieved data callback
{
  uint8_t temp_package_n = data[0];
  uint8_t temp_package_i = data[1];
  uint8_t BCC = 0;
  if (temp_package_i == 0)
  {
    rx_package_n = temp_package_n;
    rx_package_i = temp_package_i;
  }
  else
  {
    rx_package_i++;
  }

  for (int i = 0; i < data_size; i++)
  {
    BCC ^= data[i + 3];
    rx_mess[rx_package_i * data_size + i] = data[i + 3];
  }
  if ((rx_package_i == temp_package_i) && (BCC == data[2]))
  {
    SPISlave.setStatus(STT_OK);
  }
  else
  {
    SPISlave.setStatus(STT_ERR);
  }
  if (temp_package_i == temp_package_n - 1)
  {
    rx_mess[(rx_package_i + 1) * data_size] = 0;
    fifo.push_element(rx_mess);
    rx_package_n = 0;
    rx_package_i = 0;
  }
}
void onDataSentCallback(void)
{
}
void onStatusCallback(uint32_t stt)
{
  if (!Is_SPI_comm)
  {
    return;
  }
  if (stt == STT_ERR)
  {
    tx_package_i = 0;
  }
  if (tx_package_i == tx_package_n)
  {
    Is_SPI_comm = 0;
    tx_package_i = 0;
    tx_package_n = 0;
    return;
  }
  set_package_to_SPI();
}
void onStatusSentCallback(void)
{
  // Serial1.println("Status Sent");
}

void mess_handler(String json_data)
{

  DynamicJsonDocument doc(1024);
  deserializeJson(doc, json_data);
  JsonObject obj = doc.as<JsonObject>();
  String cmd = obj["cmd"];
  String device_name = obj["device"];
  int type = obj["type"];
  switch (type)
  {
  case 4:
    (DEBUGGING) ? Serial1.println("Message type is update(4)") : 0;
    send_to_master(json_data);
    break;
  case 1:
    Serial1.println("Message type is command(1)");

    for (int i = 0; i < sizeof(device) / sizeof(device[0]); i++)
    {
      if (device_name == (String)device[i].name)
      {
        Serial1.printf("Send to %s\n", device_name);
        break;
      }
    }
    break;
  case 2:
    (DEBUGGING) ? Serial1.println("Message type is response(2)") : 0;

    break;
  case 3:
    (DEBUGGING) ? Serial1.println("Message type is request(3)") : 0;

    for (int i = 0; i < sizeof(device) / sizeof(device[0]); i++)
    {
      if (device_name == (String)device[i].name)
      {
        (DEBUGGING) ? Serial1.printf("Send to %s\n", device_name) : 0;

        sendmsg(json_data, device[i].node_id);
        break;
      }
    }
    break;
  default:
    (DEBUGGING) ? Serial1.println("Not belong to any type") : 0;

    break;
  }
}

void setup()
{
  Serial1.begin(115200);
  pinMode(1, OUTPUT);
  digitalWrite(1, HIGH);
  //======MESH_init_begin==================
  mesh.setDebugMsgTypes(ERROR | STARTUP);
  mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT);
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
  mesh.setRoot();
  //======MESH_init_end==================}
  //======SPI_Slave_init_begin===========
  SPISlave.onData(&onDataCallback);
  SPISlave.onDataSent(&onDataSentCallback);
  SPISlave.onStatus(&onStatusCallback);
  SPISlave.onStatusSent(&onStatusSentCallback);
  SPISlave.begin();

  //======SPI_Slave_init_end=============
}
void loop()
{
  mesh.update();
  if (!fifo.isEmpty())
  {
    mess_handler(fifo.get_element());
  }

  if (Is_SPI_comm && (tx_timeout <= millis()))
  {
    (DEBUGGING) ? (Serial1.println("Send to FB timeout")) : 0;
    Is_SPI_comm = 0;
    tx_package_i = 0;
    tx_package_n = 0;
  }
}
