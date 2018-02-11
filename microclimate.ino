#include "config.h"

#include <ESP8266WiFi.h>
#include "RestClient.h"
#include <ArduinoJson.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

// pin connected to DH22 data line
#define DATA_PIN 2

// create DHT22 instance
DHT_Unified dht(DATA_PIN, DHT22);

class Geeks
{
    // Access specifier
    public:
 
      // Data Members
      String geekname;
   
      // Member Functions()
      void printname()
      {
         Serial.println("Geekname is: " + geekname);
      }
};

RestClient client = RestClient(REST_HOST, REST_PORT);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(100);

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // initialize dht22
  dht.begin();


}

int value = 0;

void loop() {
  // put your main code here, to run repeatedly:
//  delay(5000);
  ++value;

//  String response = "";
//  int statusCode = client.get("/api/things", &response);
//
//  //Serial.println("statusCode: " + statusCode);
//  Serial.println("response: " + response);

  // Declare an object of class geeks
  Geeks obj1;
 
  // accessing data member
  obj1.geekname = "Brent";
 
  // accessing member function
  obj1.printname();

  sensors_event_t event;
  dht.temperature().getEvent(&event);

  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();

  root["device"] = DEVICE_NAME;
  
  root["temperature"] = event.temperature;

  float celsius = event.temperature;
  float fahrenheit = (celsius * 1.8) + 32;

  dht.humidity().getEvent(&event);

  root["humidity"] = event.relative_humidity;

  String jsonStr;
  root.printTo(jsonStr);

  client.setHeader("Content-Type: application/json");

  Serial.println(jsonStr.c_str());
  String response = "";
  int statusCode = client.post("/api/readings", jsonStr.c_str(), &response);
  Serial.println("response: " + response);

  delay(READING_RATE);

}


