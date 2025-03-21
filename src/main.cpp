#include <Arduino.h>
#include <Servo.h>
#include <PubSubClient.h>
#include <WiFi.h>

// put function declarations here:
Servo servo;

const char* ssid = "iPhone";
const char* pass = "teste123";

const char* broker = "9ba2cb32944a4266a47c6f2a46d1bd26.s1.eu.hivemq.cloud";
const int port = 8883;

const char* mqtt_user = "hivemq.webclient.1742579763796";
const char* mqtt_pass = "G4aAB&8d9Q*5wU,Zv.so";

const char* topic = "servo";

WiFiClient client;
PubSubClient mqttClient(client);

void setup() {
  servo.attach(20);
  Serial.begin(9600);

  mqttClient.setServer(broker, port);
  
  WiFi.begin(ssid, pass);
  Serial.println("Connecting to Wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Wifi Connected");

  mqttClient.setServer(broker, port);
  mqttClient.connect("ESP32-Servo",mqtt_user,mqtt_pass);
  Serial.println("Connecting to the Broker...");
  while (!mqttClient.connected()) {
    Serial.println(".");
  }
  Serial.println("Broker Connected");
}

void loop() {
  
}