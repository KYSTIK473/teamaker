#include <ESP8266WiFi.h>
#define WIFI_SSID "chv"
#define WIFI_PASS "20772077"
void setup() {
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected");
  Serial.println(WiFi.localIP());
}
void loop() {
}
