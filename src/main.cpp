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

#define EVNT_OK                   0b00000000
#define EVNT_FIREBASE_ERROR       0b01000000
#define EVNT_NO_WIFI              0b00000001
#define EVNT_READ_NEUTRAL         0b00000010
#define EVNT_READ_ACID            0b00000100
#define EVNT_READ_BASE            0b00001000

constexpr uint8_t pinWifiLed = 16, pinNeutral = 27, pinAcidic = 26, pinBase = 25;


TaskHandle_t t1;
uint8_t eventData = 0;


const uint16_t wifiBlinkDelay = 10000;
int wifiBlinkTime = wifiBlinkDelay;
uint32_t wifiSleepTime = 0;
uint16_t wifiLedPinState = 1;

const uint16_t indicatorBlinkDelay = 2000;
int indicatorBlinkTime = indicatorBlinkDelay;
uint32_t indicatorSleepTime = 0;



void blinkLED(void* p) {


  uint8_t t = *(uint8_t*)p;
 
  uint16_t indicatorLedPinState = 0;
  const uint16_t mDelay = 200;
  
  while (true) {
    vTaskDelay( 1 / portTICK_PERIOD_MS );
    t = *(uint8_t*)p;

    //delay(1000);
    //Serial.print("Data: ");
   // Serial.println(t);
    if (t == EVNT_NO_WIFI) {
      delay(mDelay);
      digitalWrite(pinWifiLed, wifiLedPinState); 
      wifiLedPinState = (wifiLedPinState == 1) ? 0 : 1;
      delay(mDelay);
    }
    else{
      digitalWrite(pinWifiLed, LOW); 
    }
    
    if (t == EVNT_READ_NEUTRAL) {
      //Serial.println("Read n");
      delay(mDelay);
      digitalWrite(pinNeutral, indicatorLedPinState); 
      digitalWrite(pinAcidic, LOW);
      digitalWrite(pinBase, LOW);
      indicatorLedPinState = (indicatorLedPinState == 1) ? 0 : 1;
      delay(mDelay);

     
    }

    if (t == EVNT_READ_ACID) {
     // Serial.println("Read a");
      delay(mDelay);
      digitalWrite(pinAcidic, indicatorLedPinState); 
      digitalWrite(pinNeutral, LOW);
      digitalWrite(pinBase, LOW);
      indicatorLedPinState = (indicatorLedPinState == 1) ? 0 : 1;
      delay(mDelay);
      
    }

    if (t == EVNT_READ_BASE) {
     // Serial.println("Read b");
      delay(mDelay);
      digitalWrite(pinBase, indicatorLedPinState); 
      digitalWrite(pinNeutral, LOW);
      digitalWrite(pinAcidic, LOW);
      indicatorLedPinState = (indicatorLedPinState == 1) ? LOW : HIGH;
      delay(mDelay);

    }

  }
  

  Serial.println("Exiting....");


}

void setup() {
  pinMode(pinWifiLed, OUTPUT);
  pinMode(pinNeutral, OUTPUT);
  pinMode(pinAcidic,  OUTPUT);
  pinMode(pinBase,    OUTPUT);
 
  eventData |= EVNT_NO_WIFI;
  
  digitalWrite(pinAcidic, HIGH);
  digitalWrite(pinBase, HIGH);
  digitalWrite(pinNeutral, HIGH);

  xTaskCreatePinnedToCore(blinkLED, "blink", 10000,&eventData, 1, &t1, 0);
  Serial.begin(115200);
  WiFiManager wm;
  wm.autoConnect("PH_SENSOR", "admin123");
  digitalWrite(pinWifiLed, LOW); 
  eventData = EVNT_OK;
  config.api_key      = API_KEY;
  config.database_url = DATABASE_URL;
  Firebase.reconnectNetwork(true);
  fbdo.setResponseSize(2048);
  fbdo.setBSSLBufferSize(4096, 1024);
  config.token_status_callback = tokenStatusCallback;
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("Signup success!");
    eventData = EVNT_OK;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
    eventData |= EVNT_FIREBASE_ERROR;
  }
  Firebase.begin(&config, &auth);
  Firebase.setDoubleDigits(5);
  Firebase.reconnectNetwork(true);
  

  if(!ads.begin()){
    Serial.println("Error ADS!");
    eventData |= EVNT_FIREBASE_ERROR;
  }
 
  

}




void loop() {

  
  
  if(Firebase.ready()) {
    
    float vcc = ads.computeVolts( ads.readADC_SingleEnded(0));
    float ph = (15 - ((vcc / 5.0) * 15)) - 0.5;

    //Serial.print("vcc: "); Serial.println(vcc);
    //Serial.print("Ph: "); Serial.println(ph);
   // Serial.printf("Set float... %s\n", Firebase.RTDB.setFloat(&fbdo, F("/ph/current"), ph) ? "ok" : fbdo.errorReason().c_str());
    Firebase.RTDB.setFloat(&fbdo, F("/ph/current"), ph);
    if(ph >= 6.0  && ph <= 7.9){
      eventData = EVNT_READ_NEUTRAL;
    }
    else if(ph < 6.0) {
      eventData = EVNT_READ_ACID;
    }
    else if(ph > 7.9) {
      eventData = EVNT_READ_BASE;
    }
    Serial.println(eventData);
  }


  



  //Serial.print("Ph: ");
 // Serial.println(ph);
 // Serial.println("Waiting...");
  delay(3100);
}

