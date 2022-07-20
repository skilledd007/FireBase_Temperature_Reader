#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 25

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);
/**
 * Created by K. Suwatchai (Mobizt)
 *
 * Email: k_suwatchai@hotmail.com
 *
 * Github: https://github.com/mobizt/Firebase-ESP-Client
 *
 * Copyright (c) 2022 mobizt
 *
 */

// This example shows how to increase the value at document field. This operation required Email/password, custom or OAUth2.0 authentication.

#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif

#include <Firebase_ESP_Client.h>

// Provide the token generation process info.
#include <addons/TokenHelper.h>

/* 1. Define the WiFi credentials */
#define WIFI_SSID "Farleyswifi"
#define WIFI_PASSWORD "Edgeland"

/* 2. Define the API Key */
#define API_KEY "AIzaSyDQTbY34WsONXD-sYdMDGqrN04BDfajiVU"

/* 3. Define the project ID */
#define FIREBASE_PROJECT_ID "temperature-reader-82774"

/* 4. Define the user Email and password that alreadey registerd or added in your project */
#define USER_EMAIL "outsidetempreader@esp.wifi"
#define USER_PASSWORD "123456"

// Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long dataMillis = 0;
int count = 0;
double temperatures[5] = {25,35,30,21};
void setup()
{

    Serial.begin(115200);
    sensors.begin();
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(300);
    }
    Serial.println();
    Serial.print("Connected with IP: ");
    Serial.println(WiFi.localIP());
    Serial.println();

    Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

    /* Assign the api key (required) */
    config.api_key = API_KEY;

    /* Assign the user sign in credentials */
    auth.user.email = USER_EMAIL;
    auth.user.password = USER_PASSWORD;

    /* Assign the callback function for the long running token generation task */
    config.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h

#if defined(ESP8266)
    // In ESP8266 required for BearSSL rx/tx buffer for large data handle, increase Rx size as needed.
    fbdo.setBSSLBufferSize(1024 /* Rx buffer size in bytes from 512 - 16384 */, 4096 /* Tx buffer size in bytes from 512 - 16384 */);
#endif

    // Limit the size of response payload to be collected in FirebaseData
    fbdo.setResponseSize(2048);

    Firebase.begin(&config, &auth);

    Firebase.reconnectWiFi(true);
}

void loop()
{
   
    Serial.println("Count:  ");
    Serial.println(count);
    // Firebase.ready() should be called repeatedly to handle authentication tasks.
      
    if (Firebase.ready() && (millis() - dataMillis > 2000 || dataMillis == 0))
    {
        dataMillis = millis();
        double temp = sensors.getTempCByIndex(0);
       String documentPath = "Temperature/Outside";
       FirebaseJson content;
       content.set("fields/temperature/doubleValue",String(temp).c_str());
       if(Firebase.Firestore.patchDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath.c_str(), content.raw(), "temperature")){
      Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
      if(count == 4)
      {
          count = 0;
      } else {
          count++;
      } 
      return;
    }else{
      Serial.println(fbdo.errorReason());
    }
    if(Firebase.Firestore.createDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath.c_str(), content.raw())){
      Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
      return;
    }else{
      Serial.println(fbdo.errorReason());
    }
      
           
    }
    

   
}
