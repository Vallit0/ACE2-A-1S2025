#include <WiFi.h>
#include <PubSubClient.h>
#include <NewPing.h>

// Configuraciones de la red WiFi
const char* ssid = "Familia Valle Bances";
const char* password = "16090607VB";

// Configuraciones del servidor MQTT
const char* mqtt_server = "broker.hivemq.com";
const char* mqtt_topic = "clasesita";

WiFiClient espClient;
PubSubClient client(espClient);

// Pines del sensor ultrasónico
#define TRIGGER_PIN  5
#define ECHO_PIN     18
#define MAX_DISTANCE 200  // Distancia máxima en centímetros

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.println("Dirección IP: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Intentando conectar al servidor MQTT...");
    if (client.connect("ESP32Client")) {
      Serial.println("conectado");
    } else {
      Serial.print("fallido, rc=");
      Serial.print(client.state());
      Serial.println(" intentando de nuevo en 5 segundos");

    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Leer la distancia del sensor ultrasónico
  unsigned int distance = sonar.ping_cm();

  if (distance > 0 && distance < 20) {  // Detectar objetos a menos de 20 cm
    Serial.print("Objeto detectado a ");
  Serial.print(distance);
  Serial.println(" cm");

  // Convertir la distancia a una cadena
  String distanceStr = String(distance);

  // Publicar la distancia como mensaje MQTT
  client.publish(mqtt_topic, distanceStr.c_str());
  }

}
