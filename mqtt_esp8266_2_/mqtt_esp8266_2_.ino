#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "SMUHERO.LAB";
const char* password = "Smuhero@LAB";
const char* mqtt_server = "broker.hivemq.com";

#define LED_PIN 5

// Inisialisasi
WiFiClient espClient;
PubSubClient client(espClient);

// Fungsi untuk koneksi WiFi
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

  Serial.println("");
  Serial.println("WiFi terhubung");
  Serial.print("Alamat IP: ");
  Serial.println(WiFi.localIP());
}

// Callback untuk menerima pesan dari broker
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Pesan diterima [");
  Serial.print(topic);
  Serial.print("] ");

  String msg = "";
  for (int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }
  Serial.println(msg);

  if (msg == "1") {
    digitalWrite(LED_PIN, HIGH);  // LED ON
    client.publish("bagas/1", "1", true);  // Kirim status ON
  } else {
    digitalWrite(LED_PIN, LOW); // LED OFF
    client.publish("bagas/1", "0", true);  // Kirim status OFF
  }
}

// Fungsi reconnect MQTT
void reconnect() {
  while (!client.connected()) {
    Serial.print("Menghubungkan ke MQTT...");
    String clientId = "ESP8266Client-" + String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("terhubung!");
      client.subscribe("coba/123");
      Serial.println("Subscribe ke topik coba/123");

      // Kirim status terakhir saat pertama konek (opsional)
      bool ledState = digitalRead(LED_PIN) == HIGH;
      client.publish("bagas/1", ledState ? "1" : "0", true);
    } else {
      Serial.print("Gagal, rc=");
      Serial.print(client.state());
      Serial.println(" coba lagi dalam 5 detik");
      delay(5000);
    }
  }
}

void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW); // LED OFF default

  Serial.begin(115200);
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
