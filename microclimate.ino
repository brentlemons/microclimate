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

int loopPos = 0;

void loop() {
  // put your main code here, to run repeatedly:
//  delay(5000);

//  String response = "";
//  int statusCode = client.get("/api/things", &response);
//
//  //Serial.println("statusCode: " + statusCode);
//  Serial.println("response: " + response);
  StaticJsonBuffer<200> jsonBuffer;

  if (++loopPos >= 6) {

    int rawLevel = analogRead(A0);

    // the 10kΩ/47kΩ voltage divider reduces the voltage, so the ADC Pin can handle it
    // According to Wolfram Alpha, this results in the following values:
    // 10kΩ/(47kΩ+10kΩ)*  5v = 0.8772v
    // 10kΩ/(47kΩ+10kΩ)*3.7v = 0.649v
    // 10kΩ/(47kΩ+10kΩ)*3.1v = 0.544
    // * i asumed 3.1v as minimum voltage => see LiPO discharge diagrams
    // the actual minimum i've seen was 467, which would be 2.7V immediately before automatic cutoff
    // a measurement on the LiPo Pins directly resulted in >3.0V, so thats good to know, but no danger to the battery.

    // convert battery level to percent
    int level = map(rawLevel, 500, 649, 0, 100);

    // i'd like to report back the real voltage, so apply some math to get it back
    // 1. convert the ADC level to a float
    // 2. divide by (R2[1] / R1 + R2)
    // [1] the dot is a trick to handle it as float
    float realVoltage = (float)rawLevel / 1000 / (10000. / (47000 + 10000));
    
//    // build a nice string to send to influxdb or whatever you like
//    char dataLine[64];
//    // sprintf has no support for floats, but will be added later, so we need a String() for now
//    sprintf(dataLine, "voltage percent=%d,adc=%d,real=%s,charging=%d\n",
//        level < 150 ? level : 100, // cap level to 100%, just for graphing, i don't want to see your lab, when the battery actually gets to that level
//        rawLevel,
//        String(realVoltage, 3).c_str(),
//        rawLevel > 800 ? 1 : 0 // USB is connected if the reading is ~870, as the voltage will be 5V, so we assume it's charging
//    );

//    Serial.println(dataLine);

    JsonObject& statusRoot = jsonBuffer.createObject();
  
    statusRoot["device"] = DEVICE_NAME;
    
    statusRoot["percentage"] = level < 150 ? level : 100;
    statusRoot["adc"] = rawLevel;
    statusRoot["realVoltage"] = realVoltage;
    statusRoot["charging"] = rawLevel > 800 ? 1 : 0;
  
    String statusJson;
    statusRoot.printTo(statusJson);
  
    Serial.println(statusJson.c_str());
    String response = "";
    client.setHeader("Content-Type: application/json");
    int statusCode = client.post("/api/status", statusJson.c_str(), &response);
    Serial.println("statusresponse: " + response);

    loopPos = 0;
  }

  // Declare an object of class geeks
  Geeks obj1;
 
  // accessing data member
  obj1.geekname = "Brent";
 
  // accessing member function
  obj1.printname();

  sensors_event_t event;
  dht.temperature().getEvent(&event);

//  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();

  root["device"] = DEVICE_NAME;  
  root["temperature"] = event.temperature;

  float celsius = event.temperature;
  float fahrenheit = (celsius * 1.8) + 32;

  dht.humidity().getEvent(&event);

  root["humidity"] = event.relative_humidity;

  String jsonStr;
  root.printTo(jsonStr);


  Serial.println(jsonStr.c_str());
  String response = "";
  client.setHeader("Content-Type: application/json");
  int statusCode = client.post("/api/readings", jsonStr.c_str(), &response);
  Serial.println("reading response: " + response);

  delay(READING_RATE);

}


