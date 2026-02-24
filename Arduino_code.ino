/*
  Environmental Monitoring Station - Arduino Mega 
  Description:
  This module handles sensor data collection and transmits it to the ESP32 via UART.

  Author: Raluca Palamiuc
  Year: 2026
 */

#include "DHT.h"

#define DHTPIN 2   //digital pin for DHT11
#define DHTTYPE DHT11  
#define RAIN_PIN A0   //Analog pin for Rain sensor
#define AIR_PIN A1    //Analog pin for MQ-135


DHT dht(DHTPIN, DHTTYPE); //initialize DTH sensor

void setup() {
  Serial.begin(115200);   
  Serial1.begin(115200);  // Initialize Serial1 for communication with ESP32 (TX1: Pin 18)
  dht.begin();
  
  Serial.println("Weather Station on");
}

void loop() {
  
  // Sensor Data Acquisition
  float t = dht.readTemperature();
  int ploaieVal = analogRead(RAIN_PIN);
  int aerVal = analogRead(AIR_PIN); 

  // Rain Status Logic
  String statusPloaie = (ploaieVal < 500) ? "YES" : "NO";
  // Format: T:Temperature|R:Rain|A:Air
  String pachetDate = "T:" + String(t) + "|R:" + statusPloaie + "|A:" + String(aerVal);
  
  // Send the serialized packet to ESP32 via Hardware Serial1
  Serial1.println(pachetDate); 
  
  // Local monitoring via Serial Monitor
  Serial.println("Data sent to ESP: " + pachetDate);
  
  // 5-second delay for sensor stabilization (specifically MQ-135)
  delay(5000); 
}