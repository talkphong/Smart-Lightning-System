#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <BH1750.h>
#include "RTClib.h"
RTC_DS1307 rtc;
BH1750 lightMeter;


#define RX_PIN 10
#define TX_PIN 11
SoftwareSerial mySerial(RX_PIN, TX_PIN);
StaticJsonDocument<500> json;

///////////Du lieu gui di///////////
bool den1State;
bool den2State;
float rtcStart;
float rtcEnd;
float lux;
bool chuyenDong;
bool tuDong = 1;
////////////////////////////////////

///////////Cam Chan///////////
int pirPin = 12;
int btnPin = 8;
int den1Pin = 6, den2Pin = 7;
//////////////////////////////

unsigned long time1 = 0;
unsigned long time2 = 0;

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);

  pinMode(btnPin, INPUT_PULLUP);
  pinMode(den1Pin, OUTPUT);
  pinMode(den2Pin, OUTPUT);
  pinMode(pirPin, INPUT);


  lightMeter.begin();
  rtc.begin();
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
}
int preBtn = 1;

void loop() {

  int btnState = digitalRead(btnPin);

  if (btnState != preBtn) {
    if (btnState == 0) {
      tuDong = !tuDong;
    }
    preBtn = btnState;
  }

  if (tuDong == 1) {
    CheDoTuDong();
  } else {
    CheDoThuCong();
  }

  sendData();
  delay(500);
}



void CheDoThuCong() {
  Serial.println("Che Do Thu Cong   ");
  lux = lightMeter.readLightLevel();
  chuyenDong = digitalRead(pirPin);
  receiveData();
}

void CheDoTuDong() {
  Serial.println("Che Do Tu Dong   ");

  lux = lightMeter.readLightLevel();
  chuyenDong = digitalRead(pirPin);
  DateTime now = rtc.now();

  /////////////den1/////////////
  if (chuyenDong == 1) {
    time1 = millis();
  }

  time2 = millis();

  if (lux < 500) {
    if (chuyenDong == 1) {
      den1State = 1;
    }
  } else {
    den1State = 0;
  }

  if (time2 - time1 >= 2000) {
    den1State = 0;
  }

  /////////////den2/////////////
  if (now.hour() >= rtcStart && now.hour() <= rtcEnd) {
    den2State = 1;
  } else {
    den2State = 0;
  }
  digitalWrite(den1Pin, den1State);
  digitalWrite(den2Pin, den2State);
}


void sendData() {
  Serial.println(" SEND ESP ");
  json["den1State"] = den1State;
  json["den2State"] = den2State;
  json["rtcStart"] = rtcStart;
  json["rtcEnd"] = rtcEnd;
  json["lux"] = lux;
  json["chuyenDong"] = chuyenDong;
  json["tuDong"] = tuDong;

  serializeJson(json, mySerial);
  mySerial.println();
}


void receiveData() {
  if (mySerial.available()) {
    Serial.print(" RECEIVE ESP ");
    String line = mySerial.readStringUntil('\n');
    Serial.println(line);
    DeserializationError error = deserializeJson(json, line);

    den1State = json["den1State"];
    den2State = json["den2State"];
    rtcStart = json["rtcStart"];
    rtcEnd = json["rtcEnd"];
    // lux = json["lux"];
    // chuyenDong = json["chuyenDong"];
    // tuDong = json["tuDong"];

    digitalWrite(den1Pin, den1State);
    digitalWrite(den2Pin, den2State);
  }
}

void printSerial() {
  Serial.print("den1State:");
  Serial.print(den1State);
  Serial.print(" den2State:");
  Serial.print(den2State);
  Serial.print(" rtcStart:");
  Serial.print(rtcStart);
  Serial.print(" rtcEnd:");
  Serial.print(rtcEnd);
  Serial.print(" lux:");
  Serial.print(lux);
  Serial.print(" chuyenDong:");
  Serial.print(chuyenDong);
  Serial.print(" tuDong:");
  Serial.println(tuDong);
}