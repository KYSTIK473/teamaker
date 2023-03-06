#include <WiFi.h>
#include <WiFiProv.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <ArduinoJson.h>
#include "esp32-hal-log.h"

#include <cstdlib> // для strtod


static const char* TAG = "network";
static const char* TAG_EVT = "netowork-evt";


bool have_tea_request = false;
int tea_request_temp;
int tea_request_tea_portions;
int tea_request_sugar_portions;


static void on_wifi_event(arduino_event_t *sys_event)
{
    switch (sys_event->event_id) {
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
    {
        const auto addr = IPAddress(sys_event->event_info.got_ip.ip_info.ip.addr);
        ESP_LOGI(TAG_EVT, "Connected to IP address: %s", addr.toString().c_str());
    } break;
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
        ESP_LOGI(TAG_EVT, "Disconnected from AP");
        break;
    case ARDUINO_EVENT_PROV_START:
        ESP_LOGI(TAG_EVT, "Provisioning started\nGive Credentials of your access point using \" Android app \"");
        break;
    case ARDUINO_EVENT_PROV_CRED_RECV: {
        ESP_LOGI(TAG_EVT, "Received Wi-Fi credentials");
        ESP_LOGI(TAG_EVT, "SSID: '%s'", (const char *)sys_event->event_info.prov_cred_recv.ssid);
        ESP_LOGI(TAG_EVT, "password: '%s'", (const char *)sys_event->event_info.prov_cred_recv.password);
        break;
    }
    case ARDUINO_EVENT_PROV_CRED_FAIL: { 
        ESP_LOGE(TAG_EVT, "Provisioning failed. nPlease reset to factory and retry provisioning");
        if(sys_event->event_info.prov_fail_reason == WIFI_PROV_STA_AUTH_ERROR) 
            ESP_LOGE(TAG_EVT, "Wi-Fi AP password incorrect");
        else
            ESP_LOGE(TAG_EVT, "Wi-Fi AP not found....Add API \" nvs_flash_erase() \" before beginProvision()");        
        break;
    }
    case ARDUINO_EVENT_PROV_CRED_SUCCESS:
        ESP_LOGI(TAG_EVT, "Provisioning Successful");
        break;
    case ARDUINO_EVENT_PROV_END:
        ESP_LOGI(TAG_EVT, "Provisioning Ends");
        break;
    default:
        break;
    }
}


static WebServer server(80);

static void handleNotFound();
static void handleApiStatusRequest();
static void handleApiPutOrderRequest();


void setup_network()
{
   WiFi.onEvent(on_wifi_event);

  // Запускаем provisioning
  WiFiProv.beginProvision(
    WIFI_PROV_SCHEME_SOFTAP,
    WIFI_PROV_SCHEME_HANDLER_NONE,
    WIFI_PROV_SECURITY_1,
    "tasty-tea",
    "PROV_teamaker"
  );

  // Ждем соединения  ESP_LOGI(TAG, "Waiting for connection");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    ESP_LOGI(TAG, "Waiting...");
  }
  ESP_LOGI(TAG, "Connected to: %s", WiFi.SSID().c_str());
  ESP_LOGI(TAG, "IP address: %s", WiFi.localIP().toString().c_str());


  // Запускаем MDNS
  MDNS.begin("teamaker");
  MDNS.addService("http", "tcp", 80);
  ESP_LOGI(TAG, "MDNS started for \"teamaker.local\"");


  // Запускаем сервер
  server.on("/api/status", handleApiStatusRequest);
  server.on("/api/put-order", handleApiPutOrderRequest);
  
  server.onNotFound(handleNotFound);
  server.begin();
  ESP_LOGI(TAG, "web server started");
}


void loop_network()
{
  server.handleClient();
}


static void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }

  server.send(404, "text/plain", message);
}


static void send_headers()
{
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "0");
}


static void handleApiStatusRequest()
{
  const auto remote_addr = server.client().remoteIP().toString();
  ESP_LOGI(TAG, "teamaker status requested from %s", remote_addr.c_str());

  send_headers();

  DynamicJsonDocument doc(1024);
  doc["temperature"] = 100.f;
  auto active_order = doc.createNestedObject("active-order");
  if (have_tea_request)
  {
    active_order["temperature"] = tea_request_temp;
    active_order["tea_portions"] = tea_request_tea_portions;
    active_order["sugar_portions"] = tea_request_sugar_portions;
  }
  doc["mode"] = have_tea_request ? "making-tea" : "idle";
  doc["water_level"] = 10;
  doc["uptime"] = millis();

  String output;
  serializeJson(doc, output);

  server.send(200, "application/json", output);
}


static bool get_int_arg(const char * arg_name, int & target)
{
  const String raw = server.arg(arg_name);
  const char * raw_cstr = raw.c_str();
  char * raw_cstr_end = const_cast<char*>(raw_cstr);
  long int rv = std::strtol(raw_cstr, &raw_cstr_end, 0);
  if (raw_cstr_end != raw_cstr + raw.length())
    return false;

  target = rv;
  return true;
}


static void handleApiPutOrderRequest()
{
  const auto remote_addr = server.client().remoteIP().toString();
  ESP_LOGI(TAG, "teamaker order from %s", remote_addr.c_str());

  send_headers();

  // температура заварки
  int temperature = 96;
  // Количество порций заварки
  int tea_portions = 1;
  // Количество порций сахара
  int sugar_portions = 1;

  if (!get_int_arg("temperature", temperature))
  {
    server.send(500, "application/json", "{\"error\": \"bad temperataure\"}");
    ESP_LOGI(TAG, "teamaker order bad temperature");
    return;
  }

  if (!get_int_arg("tea_portions", tea_portions))
  {
    server.send(500, "application/json", "{\"error\": \"bad tea portions count\"}");
    ESP_LOGI(TAG, "teamaker order bad tea portions");
    return;
  }

  if (!get_int_arg("sugar_portions", sugar_portions))
  {
    server.send(500, "application/json", "{\"error\": \"bad sugar portions count\"}");
    ESP_LOGI(TAG, "teamaker order bad sugar portions");
    return;
  }

  if (have_tea_request)
  {
    server.send(503, "application/json", "{\"error\": \"already making tea\"}");
    ESP_LOGI(TAG, "teamaker order already working");
    return;
  }

  have_tea_request = true;
  tea_request_temp = temperature;
  tea_request_tea_portions = tea_portions;
  tea_request_sugar_portions = sugar_portions;

  server.send(200, "application/json", "{}");
  ESP_LOGI(TAG,
    "teamaker order accepted. temp: %d, tea: %d, sugar: %d",
    tea_request_temp, tea_request_tea_portions, tea_request_sugar_portions
  );
}
