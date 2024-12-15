
#include <WiFi.h>
#include <PubSubClient.h>

// WiFi credentials
const char* ssid = "Huawei-87EA";        // اسم شبكة WiFi
const char* password = "66072157"; // كلمة مرور شبكة WiFi

// MQTT Broker settings
const char* mqttServer = "broker.hivemq.com"; // خادم HiveMQ
const int mqttPort = 1883;                    // المنفذ الافتراضي
const char* mqttTopic = "sensor/distance";    // اسم الموضوع لإرسال البيانات

WiFiClient espClient;
PubSubClient client(espClient);

const int trigPin = 23;  // Pin connected to Trigger
const int echoPin = 22;  // Pin connected to Echo
float distance;

void setup() {
  Serial.begin(9600);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");

  // Connect to MQTT broker
  client.setServer(mqttServer, mqttPort);
  connectToMQTT();

  // Set up HC-SR04 pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void loop() {
  if (!client.connected()) {
    connectToMQTT();
  }
  client.loop();

  // Calculate distance
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;

  // Publish data to MQTT topic
  char message[50];
  sprintf(message, "{\"distance\": %.2f}", distance);
  client.publish(mqttTopic, message);

  Serial.print("Published distance: ");
  Serial.println(message);

  delay(1000);
}

void connectToMQTT() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");
    if (client.connect("ESP32Client")) {
      Serial.println("Connected to MQTT broker");
    } else {
      Serial.print("Failed, rc=");
      Serial.println(client.state());
      delay(2000);
    }
  }
}