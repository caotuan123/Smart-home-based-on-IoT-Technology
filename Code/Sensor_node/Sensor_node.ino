
#include "main.h"

#define DHTPIN 13
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

bool get_temp_humi(float *humidity, float *temperature, float *heatIndexCelcius);
float get_R0();

int get_ppm(float m, float b, float R0);

void receivedCallback(uint32_t from, String &msg)
{
  // Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
}

void newConnectionCallback(uint32_t nodeId)
{
  // Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback()
{
  // Serial.printf("Changed connections\n");
}

void nodeTimeAdjustedCallback(int32_t offset)
{
  // Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(), offset);
}

void setup()
{
  Serial.begin(115200);
  dht.begin();

  mesh.setDebugMsgTypes(ERROR | STARTUP); // set before init() so that you can see startup messages
  mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT);
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
  mesh.setContainsRoot();
  colectData.restart();
  Serial.println("Data collection started");
}

void loop()
{
  mesh.update();
}
bool get_temp_humi(float *humidity, float *temperature, float *heatIndexCelcius)
{
  float temp_h = dht.readHumidity();
  float temp_t = dht.readTemperature(); // in Celcius
  if (isnan(temp_h) || isnan(temp_t))
  {
    return false;
  }
  float temp_hic = dht.computeHeatIndex(temp_t, temp_h, false); // heat index
  *humidity = temp_h;
  *temperature = temp_t;
  *heatIndexCelcius = temp_hic;
  Serial.println(temp_h);
  Serial.println(temp_t);
  Serial.println(temp_hic);
  return true;
}
float get_R0()
{
  int sensorVal = analogRead(A0);
  float sensorInputVolt = 5;
  float senso_mV = (float)(1000 * sensorVal * sensorInputVolt) / 1024;
  float RS_air; //  Get the value of RS via in a clear air
  float R0;     // Get the value of R0 via in H2

  RS_air = ((float)sensorInputVolt * 1000.0 - senso_mV) / senso_mV; // omit * RL
  R0 = (float)RS_air / 9.8;                                         // 9.8 is the ratio of Rs/R0 when air fress
  return R0;
}

int get_ppm(float m, float b, float R0)
{
  int sensorVal = analogRead(A0);
  float senso_mV = (float)(1000 * sensorVal * 5) / 1024;
  float RS_air;
  RS_air = (5000.0 - senso_mV) / senso_mV;
  int x = pow(10, (log10(RS_air / R0) - b) / m);
  Serial.println(x);
  return x;
}
void dataColector(void)
{
  static float sum_measured_temp;
  static float sum_measured_hum;
  static float sum_measured_hic;
  static int sum_measured_ppm;

  float measured_temp;
  float measured_hum;
  float measured_hic;
  int measured_ppm;

  measured_ppm = get_ppm(M_LPG, B_LPG, R0_GAZ);
  (measured_ppm < 0) ? measured_ppm = 0 : 0;
  (measured_ppm > 10000) ? measured_ppm = 10000 : 0;
  bool Is_ok = get_temp_humi(&measured_hum, &measured_temp, &measured_hic);
  if (!Is_ok)
  {
    measured_temp = 0;
    measured_hum = 0;
    measured_hic = 0;
  }
  /*
  Serial.printf("NHET DO: %.2f\n", measured_temp);
  Serial.printf("DO AM: %.2f\n", measured_hum);
  Serial.printf("NHIET DO CAM NHAN: %.2f\n", measured_hic);
  Serial.printf("PPM: %d\n", measured_ppm);
  */
  sum_measured_temp += measured_temp;
  sum_measured_hum += measured_hum;
  sum_measured_hic += measured_hic;
  sum_measured_ppm += measured_ppm;

  temp_counter++;
  hum_counter++;
  hic_counter++;
  ppm_counter++;

  if (temp_counter == 12)
  {
    temp_counter = 0;
    sum_measured_temp /= 12;
    char flToChar[10];
    String mess;

    snprintf(flToChar, sizeof(flToChar), "%.2f", sum_measured_temp);
    mess = update_generate("temp_sen", "status", flToChar, INDEX_TEMP);
    mesh.sendSingle(HOST_ID, mess);
    mess.clear();

    sum_measured_temp = 0;
  }
  if (hum_counter == 12)
  {
    hum_counter = 0;
    sum_measured_hum /= 12;
    char flToChar[10];
    String mess;

    snprintf(flToChar, sizeof(flToChar), "%.2f", sum_measured_hum);
    mess = update_generate("humi_sen", "status", flToChar, INDEX_HUM);
    mesh.sendSingle(HOST_ID, mess);
    mess.clear();

    sum_measured_hum = 0;
  }
  if (hic_counter == 12)
  {
    hic_counter = 0;

    sum_measured_hic /= 12;

    char flToChar[10];
    String mess;

    snprintf(flToChar, sizeof(flToChar), "%.2f", sum_measured_hic);
    mess = update_generate("hic_sen", "status", flToChar, INDEX_HIC);
    mesh.sendSingle(HOST_ID, mess);
    mess.clear();

    sum_measured_hic = 0;
  }
  if (ppm_counter == 12)
  {
    ppm_counter = 0;
    sum_measured_ppm /= 12;
    char flToChar[10];
    String mess;

    itoa(sum_measured_ppm, flToChar, 10);
    mess = update_generate("gas_sen", "status", flToChar, INDEX_GAS_PPM);
    mesh.sendSingle(HOST_ID, mess);
    mess.clear();

    sum_measured_ppm = 0;
  }
}