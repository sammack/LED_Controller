

// LED Controller for Neopixels strips. For Home Automation
//
// Copyright (c) 2018 Sam MacKenzie
// Licensed under the MIT license.
//
// Above text shall be included in any redistribution.

// modify credentials.template to include your own Adafruit IO and wi-fi
// credentials and rename it to credentials.h


// ***********  Includes ******************
#include "credentials.h"
#include <ESP8266WiFi.h>
#include <ArduinoHttpClient.h>
#include <AdafruitIO.h>
#include <AdafruitIO_Dashboard.h>
#include <AdafruitIO_Data.h>
#include <AdafruitIO_Definitions.h>
#include <AdafruitIO_Feed.h>
#include <AdafruitIO_Group.h>
#include <AdafruitIO_MQTT.h>
#include <AdafruitIO_WiFi.h>
#include <Adafruit_NeoPixel.h>

// ************* Defines ******************
#define NEOPIXEL_PIN  15  // the pin used to control the neopixels
#define NUMPIXELS 5   // the number of neopixels
#define NEOPIXEL_DELAY 50 // the delay between neopixel updates
#define LIGHT_LOCATION 'A'


// setup Adafruit IO wifi, define credentials in credentials.h
AdafruitIO_WiFi io(ADAFRUIT_IO_USERNAME, ADAFRUIT_IO_KEY, WIFI_SSID, WIFI_PASSWORD);
AdafruitIO_Feed *message_feed = io.feed("light-strip");

// create a neopixel instance and initalize it
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800); 

void setup() {
  strip.begin();
  strip.show();

  // Start serial consol
  Serial.begin(115200);
  while(! Serial);
  Serial.print("Connecting to Adafruit IO");

  // connect to Adafruit IO
  io.connect();
  message_feed->onMessage(handleMessage);

  // wait for a connection, no timeout
  while(io.status() < AIO_CONNECTED) {
    Serial.print(io.status());
    strip.setPixelColor(1, 0,150,0);
    strip.show();
    delay(250);
    strip.setPixelColor(1, 150,0,0);
    strip.show();
    delay(250);
  }

  strip.setPixelColor(0, 0,0,100);
  strip.show();
  // Connected!
  Serial.println();
  Serial.println(io.statusText());  
}

void loop() {
  // We only need io.run() here, everything else is handled in the message
  // handlers.
  io.run();
}

// this function handles AdafruitIO messages
void handleMessage(AdafruitIO_Data *data) {
  uint16_t i;
  Serial.print("received <- ");
  Serial.println(data->value());
  String message = data->value();
  char target_light = message.charAt(0);
  Serial.println(LIGHT_LOCATION);
  Serial.println(target_light);
  if(target_light != LIGHT_LOCATION){
    return;
  }
  
  String red = message.substring(2,5);
  String green = message.substring(7,9);
  String blue = message.substring(11,13);
  
  int R = red.toInt();
  int G = green.toInt();
  int B = blue.toInt();
  
  for(i=0; i< strip.numPixels(); i++) {
    strip.setPixelColor(i, R,G,B);
  }
  strip.show();
}


