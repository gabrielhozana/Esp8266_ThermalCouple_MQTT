
#include "max6675.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define MQTT_TOPIC_TEMPERATURE "temperature"
#define MQTT_TOPIC_DSTEMP "dstemp"
#define MQTT_TOPIC_STATE "status"
#define MQTT_PUBLISH_DELAY 1000
#define MQTT_CLIENT_ID "esp8266ktc"

const char* WIFI_SSID = "Redmii";
const char* WIFI_PASSWORD = "12345678xZ";

const char *MQTT_SERVER = "192.168.43.111";
const long MQTT_PORT = 1883;
const char *MQTT_USER = "talora"; 
const char *MQTT_PASSWORD = "12345678"; 

float temperature=0.00;
//float dstemp;
//long lastMsgTime = 0;
//int timeDelay = 100;


int ktcSO = D8;
int ktcCS = D7;
int ktcCLK = D6;

MAX6675 ktc(ktcCLK, ktcCS, ktcSO);

WiFiClient espClient;
PubSubClient mqttClient(espClient);
  
void setup() {
  Serial.begin(115200);
  // give the MAX a little time to settle
  delay(500);
  
  setupWifi();
  Serial.println("wifi connected");    

  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
}

void loop() {
  if (!mqttClient.connected()) {
    mqttReconnect();
  }
  mqttClient.loop();
  
  if (WiFi.status() != WL_CONNECTED) {
    setupWifi();
  }
  
     // basic readout test
   temperature = ktc.readCelsius();
   temperature = temperature;
   
   Serial.print("Deg C = "); 
   Serial.print(temperature);
   Serial.print("t Deg F = ");
   Serial.println(ktc.readFahrenheit());
 
   delay(500);

   mqttPublish(MQTT_TOPIC_TEMPERATURE, temperature);
//   mqttClient.publish(String(temperature).c_str());

}


void mqttReconnect() {
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");

    // Attempt to connect
    if (mqttClient.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASSWORD, MQTT_TOPIC_STATE, 1, true, "disconnected", false)) {
      Serial.println("connected");

      // Once connected, publish an announcement...
      mqttClient.publish(MQTT_TOPIC_STATE, "connected", true);
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void mqttPublish(char* topic, float payload) {
  Serial.print(topic);
  Serial.print(": ");
  Serial.println(payload);

  mqttClient.publish(topic, String(payload).c_str(), true);
//  mqttClient.publish("status", "Hello from ESP8266", temperature);
//  mqttClient.publish("status", "Hello from ESP8266", temperature);
}

void setupWifi() {
  Serial.print("Connecting to WiFi network: " + String(WIFI_SSID));
//  WiFi.hostname(mqttNode.c_str());
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    // Wait 500msec seconds before retrying
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected successfully and assigned IP: " + WiFi.localIP().toString());
}
