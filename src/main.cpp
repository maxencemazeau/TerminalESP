#include <iostream>
#include<string>
#include<sstream> // for using stringstream
#include <Wire.h>
#include <Arduino.h>
#include <SPI.h>
#include "myFunctions.cpp" //fonctions utilitaires
#include <ccs811.h>
#include <MQUnifiedsensor.h>
#define GAS_SENSOR 36

#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#include <HTTPClient.h>
#include <WiFiManager.h>
WiFiManager wm;
#define WEBSERVER_H

#define LED_PIN 27 // Replace with the pin number connected to the LED



const char *mqtt_server = "172.16.5.100";
const char *mqtt_username = "mams";
const char *mqtt_password = "mams";
const int mqtt_port = 1883;

WiFiClient client;

// Create an instance of the Adafruit MQTT client
Adafruit_MQTT_Client mqtt(&client, mqtt_server, mqtt_port, mqtt_username, mqtt_password);

// Create a MQTT topic for publishing
Adafruit_MQTT_Publish topic_publish_CO2 = Adafruit_MQTT_Publish(&mqtt, "CO2");

Adafruit_MQTT_Publish topic_publish_GAZ = Adafruit_MQTT_Publish(&mqtt, "GAZ");

Adafruit_MQTT_Subscribe topic_subscribe = Adafruit_MQTT_Subscribe(&mqtt, "CO2_received");


char buffer1[64];
char buffer2[64];
char buffer3[64];
float val1, val2;

CCS811 ccs811(23);

//Variable pour la connection Wifi
const char *SSID = "SAC_";
const char *PASSWORD = "sac_";
String ssIDRandom;

void setup() {
  Serial.begin(9600);
  Wire.begin();

  // Enable CCS811
  bool ok = ccs811.begin();
  if ( !ok ) Serial.println("setup: CCS811 begin FAILED");

  // Start measuring
  ok = ccs811.start(CCS811_MODE_1SEC);
  if ( !ok ) Serial.println("setup: CCS811 start has FAILED");


  //Connection au WifiManager
    String ssIDRandom, PASSRandom;
    String stringRandom;
    stringRandom = get_random_string(4).c_str();
    ssIDRandom = SSID;
    ssIDRandom = ssIDRandom + stringRandom;
    stringRandom = get_random_string(4).c_str();
    PASSRandom = PASSWORD;
    PASSRandom = PASSRandom + stringRandom;

char strToPrint[128];
    sprintf(strToPrint, "Identification : %s   MotDePasse: %s", ssIDRandom, PASSRandom);
    Serial.println(strToPrint);


 if (!wm.autoConnect(ssIDRandom.c_str(), PASSRandom.c_str())){
        Serial.println("Erreur de connexion.");
      
        }
    else {
        Serial.println("Connexion Établie.");
        }
          
      // Connect to the MQTT broker
  if (mqtt.connected()) {
    mqtt.disconnect();
  }
  
    if (mqtt.connect()) {
      Serial.println("Connected to MQTT broker");
    } else {
      Serial.println("Could not connect to MQTT broker. Retrying...");
     
    }

     pinMode(LED_PIN, OUTPUT);

  
  
}

void subscribeToTopic(Adafruit_MQTT_Subscribe* topic) {
  if (mqtt.subscribe(topic)) {
    Serial.print(F("Subscribed to topic: "));
    Serial.println(topic->topic);
    digitalWrite(LED_PIN, LOW);

  } else {
    Serial.println(F("Failed to subscribe to topic"));
  }
}

void loop() {

  // Read
  uint16_t eco2, etvoc, errstat, raw;
  ccs811.read(&eco2, &etvoc, &errstat, &raw);

  // Print measurement results based on status
  if ( errstat == CCS811_ERRSTAT_OK )
  {
    val1 = eco2;
    val2 = etvoc;
 
    Serial.print("eco2=");
    Serial.print(val1);
    Serial.print(" ppm  ");

    // Convert the value to a char array
    sprintf(buffer1, "%f", val1);
    Serial.println("Buffer1 :");
    Serial.println(buffer1);
    topic_publish_CO2.publish(buffer1);
 

  }

  // Read gas level from sensor
  int gas_ppm = analogRead(GAS_SENSOR);
  Serial.println("Gaz");
  Serial.println(gas_ppm);
  sprintf(buffer2, "%d", gas_ppm);
  topic_publish_GAZ.publish(buffer2);

  subscribeToTopic(&topic_subscribe);


  delay(3000);
}
