/******************************* WIFI **************************************/

// the AdafruitIO_WiFi client will work with the following boards:
//   - HUZZAH ESP8266 Breakout -> https://www.adafruit.com/products/2471
//   - Feather HUZZAH ESP8266 -> https://www.adafruit.com/products/2821
//   - Feather M0 WiFi -> https://www.adafruit.com/products/3010
//   - Feather WICED -> https://www.adafruit.com/products/3056

#define WIFI_SSID       "ssid"
#define WIFI_PASS       "password"
#define REST_HOST       "yourhost"
#define REST_PORT       3000

#define READING_RATE    300000
#define DEVICE_NAME     "atrium"

// comment out the following two lines if you are using fona or ethernet
//#include "AdafruitIO_WiFi.h"
//AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WIFI_SSID, WIFI_PASS);



