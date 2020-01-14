/*
 * @brief: request relay status to arduino 
 */

#define SERIAL_EVENT

#define RXD2    16
#define TXD2    17
#define PERIOD  5000

unsigned long last_request = 0;

#ifdef SERIAL_EVENT
String inputString = "";
bool stringComplete = false;
#endif

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
