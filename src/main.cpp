#include "ESP32Servo.h"
#include "PubSubClient.h"
#include "env.h"
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

#define GPIOPIN 32

#define POSICAO_1_ANGLE 120
#define POSICAO_2_ANGLE 60


WiFiClientSecure client;
PubSubClient mqttClient(client);
Servo servo;

const char* ssid = wifi_ssid;
const char* pass = wifi_password;

const char* broker = mqtt_broker;
const int port = 8883;

const char* mqtt_user = mqtt_username;
const char* mqtt_pass = mqtt_password;

const char* topic = "servo/position";

void connectToWIFI();

void callback(char* topic, byte* message, unsigned int length);
void connectToBroker();


void setup() {
  pinMode(GPIOPIN, OUTPUT);
  servo.attach(14);

  Serial.begin(115200);
  client.setInsecure(); // Necessário para poder conectar ao broker sem um CA
  mqttClient.setServer(broker, port);
  mqttClient.setCallback(callback);

  digitalWrite(GPIOPIN, LOW);
  connectToWIFI();
  connectToBroker();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connectToWIFI();
  }
  if (!mqttClient.connected()) {
    connectToBroker();
  }
  mqttClient.loop();
}

/*-------------------------------------------------------------------------------*/

void connectToWIFI() {
  WiFi.begin(ssid, pass);
  Serial.println("Connecting to Wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    long now = millis();
    if (now % 100 == 0) {
      digitalWrite(GPIOPIN, !digitalRead(GPIOPIN));
    }
    if (now % 1000 == 0) {
      Serial.print("Status: ");
      Serial.println(WiFi.status());
    }
  }
  Serial.println("Wifi Connected");
  digitalWrite(GPIOPIN, HIGH);
  delay(1000);
  digitalWrite(GPIOPIN, LOW);
}

void connectToBroker() {
  Serial.println("Connecting to the Broker...");

  while (!mqttClient.connected()) { // Limita a 5 tentativas
    String clientId = "ESP32-Servo-" + String(random(0xffff), HEX);
    Serial.print("Attempting connection as ");
    Serial.println(clientId);
    long now = millis();
    if (mqttClient.connect(clientId.c_str(), mqtt_user, mqtt_pass)) {
      Serial.println("Connected to the Broker!");
      mqttClient.subscribe(topic);
      Serial.print("Subscribed to topic: ");
      Serial.println(topic);
      digitalWrite(GPIOPIN, HIGH);
      delay(1000);
      digitalWrite(GPIOPIN, LOW);
    }
    else {
      if (now % 500 == 0) {
        digitalWrite(GPIOPIN, !digitalRead(GPIOPIN));
      }
      Serial.print("Connection failed, code: ");
      Serial.println(mqttClient.state());
    }
  }

  if (!mqttClient.connected()) {
    Serial.println("Failed to connect after multiple attempts.");
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  String message = "";

  for (int i = 0; i < length; i++) {
    char c = (char)payload[i];
    if (!isDigit(c)) {
      mqttClient.publish("servo/status", "Valor inválido");
      return;
    }
    message += c;
  }
  if (message == "Posicao_1") {
    Serial.print("Received: ");
    Serial.println(message);
    servo.write(POSICAO_1_ANGLE);
  }
  else if (message == "Posicao_2") {
    Serial.print("Received: ");
    Serial.println(message);
    servo.write(POSICAO_2_ANGLE);
  }
  else {
    Serial.print("Received: ");
    Serial.println(message);
    Serial.println("Invalid Value!");
    mqttClient.publish("servo/status", "Valor inválido");
  }
}
