/* Copyright (C) 2023 Maxence MAZEAU
 * All rights reserved.
 *
 * Projet Qualite de l'air
 * Ecole du Web
 * Projet technologique (c)2023
 *  

    Historique des versions
           Version    Date       Auteur       Description
           1.1        16/02/23  Maxence     Première version
           1.2        23/03/23  Maxence     Deuxième version
           1.3       30/04/23   Maxence     Troisième version
   
 * */

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


char buffer1[64];
char buffer2[64];
char buffer3[64];
float val1, val2;
std::string message;

CCS811 ccs811(23);

//Variable pour la connection Wifi
const char *SSID = "SAC_";
const char *PASSWORD = "sac_";
String ssIDRandom;

void setup() {
  Serial.begin(9600);
  Wire.begin();

  // Set static IP address
  IPAddress staticIP(172, 16, 6, 140);
  IPAddress gateway(172,16,4,2);
  IPAddress subnet(255,255,252,0);
  WiFi.config(staticIP, gateway, subnet);

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

     digitalWrite(LED_PIN, LOW);



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


  delay(5000);
}