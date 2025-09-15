#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

const char* ssid = "Wokwi-GUEST";
const char* password = "";

const char* mqtt_server = "test.mosquitto.org";
const char* mqtt_topic = "amcc/iot/intermediate/suhu";

#define DHTPIN 22
#define DHTTYPE DHT22

WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();
  
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println(" WIFI Terhubung!");
  client.setServer(mqtt_server, 1883);
}

void reconnect() {
  while(!client.connected()) {
    Serial.println("Mencoba Terhubung ke MQTT Broker...");
    if(!client.connect("WokwiClient-AMCC-123")) {
      Serial.println(" Terhubung");
    } else {
      Serial.print(" Gagal, kode error = ");
      Serial.print(client.state());
      Serial.println(" Coba lagi dalam 5 detik");
      delay(5000);
    }
  }
}

void loop() {
  if(!client.connected()) {
    reconnect();
  }

  client.loop();
  delay(10000);

  float t = dht.readTemperature();

  if(isnan(t)) {
    Serial.println("Gagal Membaca Sensor!");
    return;
  }

  char json_payload[30];
  snprintf(json_payload, sizeof(json_payload), "{\"temperature\":%.2f}", t);

  client.publish(mqtt_topic, json_payload);

  Serial.print("Pesan Terkirim -> Topic: ");
  Serial.print(mqtt_topic);
  Serial.print(" | Isi: ");
  Serial.println(json_payload);
}
