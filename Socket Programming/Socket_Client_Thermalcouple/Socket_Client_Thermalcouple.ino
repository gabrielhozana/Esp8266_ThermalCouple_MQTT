#include "max6675.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// IoT platform Credentials
String node = "Node1";

String logServer = "10.0.6.218";//Socket Server
int logServerPort = 8090;//Port Socket Server

// Internet router credentials
const char* WIFI_SSID = "<SSID WIFI>";
const char* WIFI_PASSWORD = "<PASSWORD WIFI>";
int timeDelay = 1000;

String data;

float temperature;

int ktcSO = D8;
int ktcCS = D7;
int ktcCLK = D6;

MAX6675 ktc(ktcCLK, ktcCS, ktcSO);
WiFiClient client;

void setup() {
  Serial.begin(115200);
  // give the MAX a little time to settle
  delay(500);
  
  setupWifi();
  Serial.println("wifi connected");    

}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    setupWifi();
    
  }
  temperature = ktc.readCelsius();
  temperature = temperature;
  Serial.println(temperature); 
  
  buildData();
  sendDataToServer(data);
  Serial.println("send GET request"); 
  delay(timeDelay);  
}

void buildData() {
  data = node+",";
  data += "temperature=";
  data += String(temperature);  
}

//Send data to Socket Server 
void sendDataToServer(String _data){  
  Serial.println("connecting to Log Server: " + String(logServer));
  //client connect to Socket Server     

  if (client.connect(logServer, logServerPort)) {
    Serial.println("succesfully connected"); 
    Serial.println("sending data...");
  //============================================
  /*
    client.print("POST /update HTTP/1.1\n");
    client.print("Host: "+String(logServer)+"\n");    
    client.print("X-SENSOR-GATEWAY-API-KEY-WSN: " + node + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(_data.length());
    Serial.println("Content length : "+String(_data.length()));
    client.print("\n");
  */  
  //==================================================    
    client.print(_data);
  } 
  client.stop();
  Serial.println("stopping the client");
  delay(timeDelay);   
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
