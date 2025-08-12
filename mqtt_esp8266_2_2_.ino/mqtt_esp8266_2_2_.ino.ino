#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "SMUHERO.LAB";
const char* password = "Smuhero@LAB";
const char* mqtt_server = "broker.hivemq.com";

// Definisi GPIO untuk 3 lampu
#define LAMP1_PIN 5   // D1
#define LAMP2_PIN 4   // D2
#define LAMP3_PIN 0  // D5

// Topik kontrol dan status untuk tiap lampu
const char* topic_ctrl_1 = "coba/lampu1";
const char* topic_ctrl_2 = "coba/lampu2";
const char* topic_ctrl_3 = "coba/lampu3";

const char* topic_stat_1 = "status/lampu1";
const char* topic_stat_2 = "status/lampu2";
const char* topic_stat_3 = "status/lampu3";

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Menghubungkan ke ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi terhubung");
  Serial.print("Alamat IP: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  String msg = "";
  for (int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }

  Serial.print("Pesan diterima [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(msg);

  int state = (msg == "1") ? HIGH : LOW;

  if (strcmp(topic, topic_ctrl_1) == 0) {
    digitalWrite(LAMP1_PIN, state);
    client.publish(topic_stat_1, msg.c_str(), true);
  } 
  else if (strcmp(topic, topic_ctrl_2) == 0) {
    digitalWrite(LAMP2_PIN, state);
    client.publish(topic_stat_2, msg.c_str(), true);
  } 
  else if (strcmp(topic, topic_ctrl_3) == 0) {
    digitalWrite(LAMP3_PIN, state);
    client.publish(topic_stat_3, msg.c_str(), true);
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Menghubungkan ke MQTT...");
    String clientId = "ESP8266Client-" + String(random(0xffff), HEX);

    if (client.connect(clientId.c_str())) {
      Serial.println("Berhasil terhubung!");

      // Subscribe ke semua topik kontrol
      client.subscribe(topic_ctrl_1);
      client.subscribe(topic_ctrl_2);
      client.subscribe(topic_ctrl_3);
      Serial.println("Berlangganan ke topik kontrol lampu.");

      // Kirim status awal semua lampu
      client.publish(topic_stat_1, digitalRead(LAMP1_PIN) ? "1" : "0", true);
      client.publish(topic_stat_2, digitalRead(LAMP2_PIN) ? "1" : "0", true);
      client.publish(topic_stat_3, digitalRead(LAMP3_PIN) ? "1" : "0", true);

    } else {
      Serial.print("Gagal. rc=");
      Serial.print(client.state());
      Serial.println(" coba lagi dalam 5 detik.");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(LAMP1_PIN, OUTPUT);
  pinMode(LAMP2_PIN, OUTPUT);
  pinMode(LAMP3_PIN, OUTPUT);

  digitalWrite(LAMP1_PIN, LOW);
  digitalWrite(LAMP2_PIN, LOW);
  digitalWrite(LAMP3_PIN, LOW);

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
