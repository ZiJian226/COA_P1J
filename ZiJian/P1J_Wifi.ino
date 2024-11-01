#include <WiFi101.h>
#include <PubSubClient.h>

// WiFi credentials
const char* ssid = "Makoto";
const char* password = "Azusa226";

// MQTT Broker settings
const char* mqtt_server = "test.mosquitto.org";
const int mqtt_port = 1883;
const char* mqtt_topic = "coap1j";
const char* mqtt_client_id = "TinyZero_Client";

// Initialize clients
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

// Variables for sensor data
float temperature = 0;
unsigned long lastPublishTime = 0;
const unsigned long publishInterval = 30000;  // Publish every 30 seconds

// Define Serial object based on which TinyCircuits processor board is used.
#if defined(ARDUINO_ARCH_SAMD)
  #define SerialMonitorInterface SerialUSB
#else
  #define SerialMonitorInterface Serial
#endif

void setup() {
  SerialMonitorInterface.begin(115200);
  WiFi.setPins(8, 2, A3, -1); // VERY IMPORTANT FOR TINYDUINO
  delay(2000);  // Give time for serial monitor to open
  
  // Initialize WiFi
  setupWiFi();
  
  // Configure MQTT broker
  mqttClient.setServer(mqtt_server, mqtt_port);
}

void loop() {
  // Ensure WiFi connection
  if (!WiFi.status() == WL_CONNECTED) {
    setupWiFi();
  }
  
  // Ensure MQTT connection
  if (!mqttClient.connected()) {
    reconnectMQTT();
  }
  
  // Handle MQTT communication
  mqttClient.loop();
  
  // Publish data periodically
  if (millis() - lastPublishTime > publishInterval) {
    publishData();
    lastPublishTime = millis();
  }
}

void setupWiFi() {
  SerialMonitorInterface.print("Connecting Wifi: ");
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    SerialMonitorInterface.print(".");
  }
  
  SerialMonitorInterface.println("\nWiFi connected");
  SerialMonitorInterface.println("IP address: ");
  SerialMonitorInterface.println(WiFi.localIP());
}

void reconnectMQTT() {
  while (!mqttClient.connected()) {
    SerialMonitorInterface.println("Attempting MQTT connection...");
    
    if (mqttClient.connect(mqtt_client_id)) {
      SerialMonitorInterface.println("Connected to MQTT broker");
    } else {
      SerialMonitorInterface.print("Failed, rc=");
      SerialMonitorInterface.print(mqttClient.state());
      SerialMonitorInterface.println(" Retrying in 5 seconds...");
      delay(5000);
    }
  }
}

void publishData() {
  // Simulate sensor reading (replace with actual sensor code)
  temperature += 0.5;
  if (temperature > 30) temperature = 20;
  
  // Create JSON message
  char message[50];
  snprintf(message, sizeof(message), "{\"temperature\":%.2f}", temperature);
  
  // Publish to MQTT topic
  if (mqttClient.publish(mqtt_topic, message)) {
    SerialMonitorInterface.println("Published: " + String(message));
  } else {
    SerialMonitorInterface.println("Failed to publish message");
  }
}
