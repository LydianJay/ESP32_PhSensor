#include <Arduino.h>
#include <Firebase_ESP_Client.h>
#include <WiFiManager.h>
#include "credentials.h"
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;


constexpr uint8_t pinPHSensor = 27;

void setup() {
  Serial.begin(115200);
  pinMode(pinPHSensor, INPUT);
  /*WiFiManager wm;
  wm.autoConnect("GSMtracker", "admin123");
  
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  Firebase.reconnectNetwork(true);
  fbdo.setResponseSize(2048);

  Firebase.begin(&config, &auth);
  */


}




void loop() {
  uint16_t aRead = analogRead(pinPHSensor);
  float miliVolt = (float)aRead * (3.3/4095.0);
  float ph = (miliVolt / 3.3) * 15;

  Serial.print("ADC: "); Serial.println(aRead);
  Serial.print("miliVolt: "); Serial.println(miliVolt);
  Serial.print("PH: "); Serial.println(ph);

  //Serial.print("Ph: ");
 // Serial.println(ph);
  delay(1000);
}

