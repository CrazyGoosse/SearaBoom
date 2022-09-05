/**
 * Mini-Web Radio V1.3
 * ======================
 * Created by Allan Gallop
 * For Milton Keynes Hospital Radio
 */

#include <Arduino.h>
#include <SPIFFS.h>
#include "mwr_radio.h";
#include "mwr_config.h";

// Pin Definitions
#define I2S_DOUT 25
#define I2S_BCLK 27
#define I2S_LRC  26
#define I2S_GAIN 12
#define I2S_SD 13
#define LED_WIFI 18
#define VOL 34
#define MODE_0 23
#define MODE_1 22
int VBATT = 33;

// Initalise Libaries
MWConfig mConfig;
MWRadio mRadio;

int mode = 0;   // Mode State

void setup() {
  // Start Serial for debugging
  Serial.begin(115200);
  pinMode(LED_WIFI,OUTPUT);

  // Start SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("An error has occurred while mounting SPIFFS");
  }
  Serial.println("SPIFFS mounted successfully");

  // Detect operating mode
  mode = mConfig.detectMode(MODE_0,MODE_1);
  if(!mode){   
    // Start into Config mode
    mConfig.apMode();
    while (WiFi.status() != WL_CONNECTED){flashLED(LED_WIFI,150);delay(500);}
  }else{      
    // Start into Radio mode
    mConfig.stMode();
    while (WiFi.status() != WL_CONNECTED){flashLED(LED_WIFI,150);delay(500);}
    WiFi.onEvent(WiFiStationDisconnected, ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
    // Initalise Audio
    mRadio.init(I2S_DOUT, I2S_BCLK, I2S_LRC, I2S_GAIN, I2S_SD);
    // Get and Set Station URL
    String url = mConfig.readUrlFromFile();
    mRadio.setStation(url);
  }
}

void loop() {
  if(mode){
    /** 
     *  V1 Hardware only
     *  ----------------------
     *  As battery is on mechanical switch we treat minimum volume
     *  as effectly a "charge only" mode
     */
    if(getVolume() >1){
     mRadio.setVolume(getVolume());
     mRadio.play();
    }
  }
}

// Get volume state, mapped 1-20
int getVolume()
{
  return 22;
}

// Flash an LED
void flashLED(int LED, int SPEED)
{
  digitalWrite(LED,HIGH);
  delay(SPEED);
  digitalWrite(LED, LOW);
  delay(SPEED);
}


void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info){
  Serial.println("Disconnected from WiFi access point");
  Serial.print("WiFi lost connection. Reason: ");
  //Serial.println(info.disconnected.reason);
  Serial.println("Trying to Reconnect");
  WiFi.reconnect();
}
