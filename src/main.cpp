#include <Arduino.h>
#include <Servo.h>
#include <PubSubClient.h>
#include <WiFi.h>

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

void callback(char* topic, byte* message, unsigned int length);
void mqttReconnect();

void setup() {
  servo.attach(20);
  Serial.begin(9600);
  WiFi.begin(ssid, pass);
  Serial.println("Connecting to Wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Wifi Connected");
  mqttClient.setServer(broker, port);
  mqttClient.setCallback(callback);
  mqttReconnect();
  mqttClient.subscribe(topic);

 
}
void mqttReconnect(){
  Serial.println("Connecting to the Broker...");
  while (!mqttClient.connected()) {
    if (mqttClient.connect("ESP32-Servo", mqtt_user, mqtt_pass)) {
      Serial.println(" Conectado!");
      mqttClient.subscribe(topic);
    } else {
      Serial.print(" Falha, código: ");
      Serial.print(mqttClient.state());
      Serial.println(". Tentando novamente em 5 segundos...");
      delay(5000);
    }
  }
}


void callback(char* topic, byte* payload, unsigned int length){
  String message = "";
  for (int i = 0; i < length; i++) {
    char c = (char)payload[i];
    if(!isDigit(c)){
      mqttClient.publish("servo", "Valor inválido");
      return;
    }
    message += c;
  }
  byte angle = message.toInt();
  if((angle >= 0) && (angle <= 180)){
    servo.write(angle);
  }else{
    mqttClient.publish("servo", "Valor inválido");
  }
}


void loop() {
  if (!mqttClient.connected()) {
    mqttReconnect();
  }
  mqttClient.loop();
}