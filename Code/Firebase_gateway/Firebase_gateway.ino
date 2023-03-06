/**
 * Created by K. Suwatchai (Mobizt)
 *
 * Email: k_suwatchai@hotmail.com
 *
 * Github: https://github.com/mobizt/Firebase-ESP32
 *
 * Copyright (c) 2022 mobizt
 *
 */
#include "main.h"
// Define Firebase Data object

// a függvény funkciója az üzenet feldolgozáss
void mess_handler(String mess)
{
  // Üzenet feldolgozása előtt, deszerializálni kell hogy üzenet legyen "érhető"
  (DEBUGGING) ? Serial.print("Message is: ") : 0;
  (DEBUGGING) ? Serial.println(mess) : 0;

  DynamicJsonDocument doc(1024); // az üzenet maximális hossza 1024 byte
  deserializeJson(doc, mess);
  JsonObject obj = doc.as<JsonObject>();
  String device = obj["device"];
  int type = obj["type"];
  (DEBUGGING) ? Serial.printf("Message type %d\n", type) : 0;

  switch (type)
  {
  case 4:
  {
    // üzenet frissítése a Firebase-en ha üzenet típusa frissítés
    (DEBUGGING) ? Serial.println("Message type is update(4)") : 0;

    String value_tmp = obj["value"];
    String path_tmp = obj["path"];
    String update_mess_tmp = project_path + path_tmp;

    bool Is_update_ok = Firebase.setString(fbdo, update_mess_tmp, value_tmp);

    if (Is_update_ok)
      (DEBUGGING) ? Serial.print("Update firebase..OK\n") : 0;
    else
      (DEBUGGING) ? Serial.printf("Update firebase.. %s\n\n", fbdo.errorReason().c_str()) : 0;
    break;
  }
  case 1:
    (DEBUGGING) ? Serial.println("Message type is command(1)") : 0;

    esp.writeData(mess); // Üzenet küldése a mesh gateway-nek
    break;
  case 2:

    (DEBUGGING) ? Serial.println("Message type is response(2)") : 0;
    break;
  case 3:

    (DEBUGGING) ? Serial.println("Message type is request(3)") : 0;
    esp.writeData(mess); // Üzenet küldése a mesh gateway-nek
    break;
  default:

    (DEBUGGING) ? Serial.println("The message does not belong to any type") : 0;
    break;
  }
}

void streamCallback(StreamData data)
{
  Serial.print("code in StreamData firebase, core:  ");
  Serial.println(xPortGetCoreID());
  if (data.boolData())
  {
    dataChanged = true;
    (DEBUGGING) ? Serial.printf("Received stream payload size: %d (Max. %d)\n\n", stream.payloadLength(), stream.maxPayloadLength()) : 0;
  }
}

void streamTimeoutCallback(bool timeout)
{
  if (timeout)
    Serial.println("stream timed out, resuming...\n");

  if (!stream.httpConnected())
    Serial.printf("error code: %d, reason: %s\n\n", stream.httpCode(), stream.errorReason().c_str());
}

ICACHE_RAM_ATTR void detectsMess()
{
  Serial.print("code in interrupt, core:  ");
  Serial.println(xPortGetCoreID());
  Is_new_mess_SPI = true;
}
void setup()
{

  Serial.begin(115200);
  Serial.setDebugOutput(true);

  //=====SET_UP_WIFI_begin==================
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  //=====SET_UP_WIFI_end==================
  //=====SET_UP_FIREBASE_begin==================
  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the user sign in credentials */
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  config.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h

  Firebase.begin(&config, &auth);

  Firebase.reconnectWiFi(true);

  // Recommend for ESP8266 stream, adjust the buffer size to match your stream data size
#if defined(ESP8266)
  stream.setBSSLBufferSize(2048 /* Rx in bytes, 512 - 16384 */, 512 /* Tx in bytes, 512 - 16384 */);
#endif

  if (!Firebase.beginStream(stream, project_path + streamPath))
    Serial.printf("stream begin error, %s\n\n", stream.errorReason().c_str());

  Firebase.setStreamCallback(stream, streamCallback, streamTimeoutCallback);

  String start_file; // prepare for first update to firebase
  StaticJsonDocument<500> doc;

  JsonArray device = doc.createNestedArray("list_device");

  JsonObject device_0 = device.createNestedObject();
  device_0["name"] = "light_0";
  device_0["status"] = "OFF";

  JsonObject device_1 = device.createNestedObject();
  device_1["name"] = "light_1";
  device_1["status"] = "OFF";

  JsonObject device_2 = device.createNestedObject();
  device_2["name"] = "rfid";
  device_2["crt_noti"] = "";
  device_2["del_noti"] = "";
  device_2["is_del"] = "0";
  device_2["is_crt"] = "0";

  JsonObject device_3 = device.createNestedObject();
  device_3["name"] = "lock";
  device_3["status"] = "CLOSE";

  JsonObject device_4 = device.createNestedObject();
  device_4["name"] = "temp_sen";
  device_4["status"] = "0";

  JsonObject device_5 = device.createNestedObject();
  device_5["name"] = "humi_sen";
  device_5["status"] = "0";

  JsonObject device_6 = device.createNestedObject();
  device_6["name"] = "hic_sen";
  device_6["status"] = "0";

  JsonObject device_7 = device.createNestedObject();
  device_7["name"] = "gas_sen";
  device_7["status"] = "0";

  doc["is_new_command"] = false;
  serializeJson(doc, start_file);
  (DEBUGGING) ? Serial.println(start_file.c_str()) : 0;

  FirebaseJson json;
  json.setJsonData(start_file);

  Serial.printf("Set json... %s\n\n", Firebase.setJSON(fbdo, project_path, json) ? "ok" : fbdo.errorReason().c_str());

  //=====SET_UP_FIREBASE_end==================
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV2); // SPI_CLOCK_DIV4= 4Mhz  SPI_CLOCK_DIV2= 8Mhz
  pinMode(22, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(22), detectsMess, FALLING);
  esp.begin();
}

void loop()
{

  // Firebase.RTDB.readStream(&stream);
  Firebase.ready();

  if (millis() > time_read_RTDB)
  {
    time_read_RTDB = millis() + 15000;
    // Firebase.getBool(fbdo,project_path + streamPath);
  }

  if (Is_new_mess_SPI)
  {
    const uint32_t timeout = (uint32_t)millis() + 200; // create timer for timeout (just in case)

    uint8_t package_i = 0;
    uint8_t package_n = 0;
    uint8_t package[32] = {0};
    uint8_t mess[1024] = {0};
    uint8_t data_size = 29;
    uint8_t BCC;
    while ((timeout >= millis()))
    {
      esp.readData((uint8_t *)package);
      if (package_i == 0)
      {
        package_i = package[1];
        package_n = package[0];
      }
      BCC = 0;
      for (int i = 0; i < data_size; i++)
      {
        BCC ^= package[i + 3];
        mess[package_i * data_size + i] = package[i + 3];
      }
      if ((BCC != package[2]) || (package_i != package[1]))
      {
        delayMicroseconds(30);
        esp.writeStatus(STT_ERR);
        delayMicroseconds(30);

        package_i = 0;
      }
      else
      {
        delayMicroseconds(30);
        esp.writeStatus(STT_OK);
        delayMicroseconds(30);

        package_i++;
        if (package_i == package_n)
        {
          fifo.push_element((char *)mess);
          break;
        }
      }
    }
    Is_new_mess_SPI = false;
  }
  if (!fifo.isEmpty())
  {
    String mess = fifo.get_element();
    mess_handler(mess);
  }

  if (dataChanged)
  {
    dataChanged = false;
    (DEBUGGING) ? Serial.print("New message up!!!!") : 0;

    if (Firebase.getArray(fbdo, project_path + path_cmd_list))
    {

      FirebaseJsonArray arr = fbdo.to<FirebaseJsonArray>();
      FirebaseJsonData json_tmp;

      (DEBUGGING) ? Serial.printf("Readed array size: %d\n", arr.size()) : 0;

      for (size_t i = 0; i < arr.size(); i++)
      {
        arr.get(json_tmp, i);
        fifo.push_element(json_tmp.to<String>().c_str());
      }
    }
    else
    {
      (DEBUGGING) ? Serial.println("There are no array to read") : 0;
    }

    Firebase.setBool(fbdo, project_path + streamPath, LOW); // idle
  }
}
