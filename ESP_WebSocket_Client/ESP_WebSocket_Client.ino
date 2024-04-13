#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>  //https://github.com/Links2004/arduinoWebSockets
WebSocketsClient webSocket;
// const char* ssid = "PhongTruong"; 
// const char* password = "33082402";
// const char* ip_host = "192.168.6.102";
const char* ssid = "bin"; 
const char* password = "21072022";
const char* ip_host = "192.168.43.21";
const uint16_t port = 3000;

#define RX_PIN 4  //D2
#define TX_PIN 5  //D1
SoftwareSerial mySerial(RX_PIN, TX_PIN);

StaticJsonDocument<500> json;

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
  Serial.println("ESP8266 Websocket Client");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  webSocket.begin(ip_host, port);
  webSocket.onEvent(receiveDataServer);
}


void loop() {
  webSocket.loop();
  receiveDataArduino();
}
void sendDataArduino() {
  Serial.print(" SEND ARDUINO ");
  serializeJson(json, mySerial);
  mySerial.println();
}


void receiveDataServer(WStype_t type, uint8_t* payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("[WSc] Disconnected!\n");
      break;
    case WStype_CONNECTED:
      {
        Serial.printf("[WSc] Connected to url: %s\n", payload);
      }
      break;
    case WStype_TEXT:
      Serial.printf("get TEXT: %s", payload);
      break;
    case WStype_BIN:
      Serial.printf("get BINARY: ");
      String strReceive = String((char*)payload);
      deserializeJson(json, strReceive);
      Serial.println(" RECEIVE SERVER ");
      sendDataArduino();
      break;
  }
}


void sendDataServer(String strSend) {
  Serial.println(" SEND SERVER ");
  webSocket.sendTXT(strSend);
}

void receiveDataArduino() {
  if (mySerial.available()) {
    Serial.println(" RECEIVE ARDUINO ");
    String line = mySerial.readStringUntil('\n');
    sendDataServer(line);
  }
}