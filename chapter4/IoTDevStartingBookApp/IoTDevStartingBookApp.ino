#include <M5StickC.h>
#include "DHT12.h"

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include "Secret.h"

static WiFiClientSecure httpsClient;
static PubSubClient mqttClient(httpsClient);

DHT12 dht12;

// WiFi接続する
void setupWifi() {
  M5.Lcd.println("connecting WiFi");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    M5.Lcd.print(".");
    delay(1000);
  }
}

// MQTT接続する
void setupMqtt() {
  M5.Lcd.println("connecting MQTT");
  httpsClient.setCACert(ROOT_CA);
  httpsClient.setCertificate(CERTIFICATE);
  httpsClient.setPrivateKey(PRIVATE_KEY);
  mqttClient.setServer(CLOUD_ENDPOINT, CLOUD_PORT);
  while (!mqttClient.connected()) {
    if (!(mqttClient.connect(CLIENT_ID))) {
      M5.Lcd.print(".");
      delay(1000);
    }
  }
}

// 気温を送信する
void publishTemperature(float tmp) {
  char message[128] = {0};
  sprintf(message, "{\"value\": %f}", tmp);
  mqttClient.publish(CLOUD_TOPIC, message);
}

void setup() {
  M5.begin();
  Wire.begin(0,26);
  M5.Lcd.setRotation(3);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0, 0, 2);
  setupWifi();
  setupMqtt();
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0, 0, 2);
  M5.Lcd.println("Current temperature.");
}

void loop() {
  float tmp = dht12.readTemperature();
  M5.Lcd.setCursor(0, 20, 2);
  M5.Lcd.printf("Temp: %2.1f", tmp);
  publishTemperature(tmp);
  delay(10000);
}
