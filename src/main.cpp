#include <Arduino.h>
#include <WiFi.h>
#include "ESP32Servo.h"
#include "PubSubClient.h"
#include "env.h"
#include <WiFiClientSecure.h>

WiFiClientSecure client;
PubSubClient mqttClient(client);
Servo servo;



const char *ssid = "TesteFerrorama";
const char *pass = "Ferrorama1";

const char *broker = "9ba2cb32944a4266a47c6f2a46d1bd26.s1.eu.hivemq.cloud";
const int port = 8883;

const char *mqtt_user = "ESPSERVO";
const char *mqtt_pass = "!ESPservo1";

const char *topic = "servo/angle";

void connectToWIFI();

void callback(char *topic, byte *message, unsigned int length);
void connectToBroker();

void setup()
{
  servo.attach(14);

  Serial.begin(115200);
  client.setInsecure(); //Necessário para poder conectar ao broker sem um CA
  mqttClient.setServer(broker, port);
  mqttClient.setCallback(callback);


  connectToWIFI();
  connectToBroker();
}

void loop()
{
  if(WiFi.status() != WL_CONNECTED)
  {
    connectToWIFI();
  }
  if (!mqttClient.connected())
  {
    connectToBroker();
  }
  mqttClient.loop();
}

/*-------------------------------------------------------------------------------*/

void connectToWIFI()
{
  WiFi.begin(ssid, pass);
  Serial.println("Connecting to Wifi...");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    Serial.print("Status: ");
    Serial.println(WiFi.status());
  }
  Serial.println("Wifi Connected");
}

void connectToBroker() {
  Serial.println("Connecting to the Broker...");

  int tentativas = 0;
  while (!mqttClient.connected() && tentativas < 5) { // Limita a 5 tentativas
    String clientId = "ESP32-Servo-" + String(random(0xffff), HEX);
    Serial.print("Attempting connection as ");
    Serial.println(clientId);

    if (mqttClient.connect(clientId.c_str(), mqtt_user, mqtt_pass)) {
      Serial.println("Connected to the Broker!");
      mqttClient.subscribe(topic);
      Serial.print("Subscribed to topic: ");
      Serial.println(topic);
    } else {
      Serial.print("Connection failed, code: ");
      Serial.println(mqttClient.state());
      Serial.println("Retrying in 2 seconds...");
      delay(2000);
      tentativas++;
    }
  }

  if (!mqttClient.connected()) {
    Serial.println("Failed to connect after multiple attempts.");
  }
}

void callback(char *topic, byte *payload, unsigned int length)
{
  String message = "";

  for (int i = 0; i < length; i++)
  {
    char c = (char)payload[i];
    if (!isDigit(c))
    {
      mqttClient.publish("servo/status", "Valor inválido");
      return;
    }
    message += c;
  }

  byte angle = message.toInt();
  if ((angle >= 0) && (angle <= 180))
  {
    Serial.print("Received: ");
    Serial.println(angle);
    servo.write(angle);
  }
  else
  {
    mqttClient.publish("servo/status", "Valor inválido");
  }
}
