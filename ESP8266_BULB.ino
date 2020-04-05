/***************************************************
  Adafruit ESP8266 Sensor Module
 ****************************************************/

// Libraries
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

// ESP8266 GPIO PIN for Google Bulb
#define GPIO_PIN_BULB 5

// WiFi parameters
#define WLAN_SSID       "ssid" // use wifi ssid
#define WLAN_PASS       "password" // use wifi password

// Adafruit IO server details.
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    "username" // user username 
#define AIO_KEY         "aio_key"  // use aio key Obtained from account info on io.adafruit.com


// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);


//Setup feed for google light
Adafruit_MQTT_Subscribe google_bulb = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/google_smart_bulb");

/*************************** Sketch Code ************************************/

void setup() {
  // Initialize the GPIO_PIN_BULB pin as an output
  pinMode(GPIO_PIN_BULB, OUTPUT);  
  pinMode(LED_BUILTIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output to show ESP8266 is running.   
  
  // set the baut rate for serial monitor.
  Serial.begin(115200);
  Serial.println(F("Google voice assistance with ESP8266 Example"));

  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  delay(10);
  Serial.print(F("Connecting to "));
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }
  Serial.println();

  Serial.println(F("WiFi connected"));
  Serial.println(F("ESP8266 IP address: "));
  Serial.println(WiFi.localIP());

// Setup MQTT subscription for Adafruit servie feed google_smart_bulb 
  mqtt.subscribe(&google_bulb);
  Serial.println(F("Suscribed to google_smart_bulb"));
  // Initialized to Bulf off mode
  digitalWrite(GPIO_PIN_BULB, LOW);
  
}

void loop() {
digitalWrite(LED_BUILTIN, HIGH);
 // ping the server to keep the mqtt connection alive
  
  Serial.println(F("Adafruit is  connected"));
// Function to connect and reconnect as necessary to the Adafruit MQTT server
  MQTT_connect();
  
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {
    // Check if its the onoff button feed
    if (subscription == &google_bulb) {
      Serial.print(F("Triggered button: "));
      Serial.println((char *)google_bulb.lastread);
      
      if (strcmp((char *)google_bulb.lastread, "ON") == 0) {
        digitalWrite(GPIO_PIN_BULB, LOW); 
      }
      if (strcmp((char *)google_bulb.lastread, "OFF") == 0) {
        digitalWrite(GPIO_PIN_BULB, HIGH); 
      }
    }
  }
  
  // Repeat every 10 seconds
  delay(5000);
}


void MQTT_connect() {
  int8_t ret;
  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }
 
  Serial.println("Connecting to Adafruit MQTT Server. ");
 
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
  }
  Serial.println("Adafruit MQTT Connected!");
}
