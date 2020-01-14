/*
 * @brief: request relay status to arduino 
 */

#define RXD2    16
#define TXD2    17
#define PERIOD  5000

unsigned long last_request = 0;

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
}

void loop() {
  if (millis() - last_request > PERIOD) {
    last_request = millis();

    Serial2.write("get_status");
    Serial2.write('\r');
  }

  // Check data received from Arduino
  while (Serial2.available()) {
    Serial.print(char(Serial2.read()));
  }
}
