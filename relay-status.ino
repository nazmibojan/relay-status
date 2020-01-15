/*
 * @brief: request relay status to arduino 
 */

#include <WiFi.h>
#include "time.h"
#include <PubSubClient.h>
#include <ArduinoJson.h>

//#define SERIAL_EVENT

#define RXD2    16
#define TXD2    17
#define PERIOD  5000

const char* ssid      = "XL-Guest";
const char* password  = "Pri[o]rita$";
const char* ntp_server = "pool.ntp.org";
const char* mqtt_server = "mqtt.flexiot.xl.co.id";
const int mqtt_port = 1883;
const char* mqtt_user = "generic_brand_2003-esp32_test-v1_3768";
const char* mqtt_pwd = "1579057622_3768";

unsigned long last_request = 0;
const long gmt_offset_sec = 25200;
const int daylight_offset_sec = 0;

#ifdef SERIAL_EVENT
String inputString = "";
bool stringComplete = false;
#endif

WiFiClient ESPClient;
PubSubClient client(ESPClient);
const char* json_hardcode = "{\"eventName\":\"event1\",\"status\":\"none\",\"temp\":22.6,\"mac\":\"3652049931932042\"}";

void printLocalTime()
{
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

void setup()
{
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);

  // Connect to WiFi
  Serial.printf("Connecting to %s", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(2000);
    Serial.print(".");
  }
  Serial.println(" CONNECTED");

  //init and get the time
  configTime(gmt_offset_sec, daylight_offset_sec, ntp_server);
  printLocalTime();

  // Connect to MQTT Server
  client.setServer(mqtt_server, mqtt_port);
  while (!client.connected()) {
    Serial.println("ESP > Connecting to MQTT...");

    if (client.connect("ESP32Client", mqtt_user, mqtt_pwd)) {
      Serial.println("Connected to FlexIoT");
    } else {
      Serial.print("ERROR > failed with state");
      Serial.print(client.state());
      Serial.print("\r\n");
      delay(2000);
    }
  }

  client.publish("generic_brand_2003/esp32_test/v1/common", json_hardcode);
}

void loop()
{
  if (millis() - last_request > PERIOD) {
    last_request = millis();

    Serial.println("get_status");
    Serial2.write("get_status");
    Serial2.write('\r');

    client.publish("generic_brand_2003/esp32_test/v1/common", json_hardcode);
  }

#ifdef SERIAL_EVENT
  if (stringComplete) {
    Serial.println(inputString);

    inputString = "";
    stringComplete = false;
    
  }
#else
  // Check data received from Arduino
  while (Serial2.available()) {
    Serial.print(char(Serial2.read()));
  }
#endif
}

#ifdef SERIAL_EVENT
/* Serial event function */
void serialEvent() {
  while (Serial2.available()) {
    char inChar = (char)Serial2.read();
    inputString += inChar;
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}
#endif
