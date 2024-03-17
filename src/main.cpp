#include <Arduino.h>
#include <Firebase_ESP_Client.h>
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>
#include <WiFiManager.h>
#include <Adafruit_ADS1X15.h>
#include "credentials.h"
FirebaseData fbdo;
Adafruit_ADS1115  ads;     
FirebaseAuth auth;
FirebaseConfig config;




void setup() {
  Serial.begin(115200);
  WiFiManager wm;
  wm.autoConnect("PH_SENSOR", "admin123");
  
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  
  Firebase.reconnectNetwork(true);
  fbdo.setResponseSize(2048);
  fbdo.setBSSLBufferSize(4096, 1024);
  config.token_status_callback = tokenStatusCallback;
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("Signup success!");
    
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }
  Firebase.begin(&config, &auth);
  Firebase.setDoubleDigits(5);
  Firebase.reconnectNetwork(true);
  

  if(!ads.begin()){
    Serial.println("Error ADS!");
  }
  Serial.print("Firebase status: "); Serial.println(Firebase.ready());
}




void loop() {

  
  
  if(Firebase.ready()) {
    
    float vcc = ads.computeVolts( ads.readADC_SingleEnded(0));
    float ph = (15 - ((vcc / 5.0) * 15)) - 0.5;

    Serial.print("vcc: "); Serial.println(vcc);
    Serial.print("Ph: "); Serial.println(ph);
    Serial.printf("Set float... %s\n", Firebase.RTDB.setFloat(&fbdo, F("/ph/current"), ph) ? "ok" : fbdo.errorReason().c_str());
  }


  



  //Serial.print("Ph: ");
 // Serial.println(ph);
 // Serial.println("Waiting...");
  delay(3100);
}

