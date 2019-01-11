#include <Arduino.h>
#include <RCSwitch.h>
#include <ESP8266WiFi.h>

#define HTTP_REST_PORT 80
#define WIFI_RETRY_DELAY 500
#define MAX_WIFI_INIT_RETRY 50

RCSwitch mySwitch = RCSwitch();

const char* wifi_ssid = "digi-led";
const char* wifi_passwd = "olecka88";

WiFiServer server(80);

int rc_delay = 50;

int init_wifi()
{
  int retries = 0;

  Serial.println("Connecting to WiFi AP..........");

  WiFi.mode(WIFI_STA);
  WiFi.begin(wifi_ssid, wifi_passwd);
  // check the status of WiFi connection to be WL_CONNECTED
  while ((WiFi.status() != WL_CONNECTED) && (retries < MAX_WIFI_INIT_RETRY))
  {
    retries++;
    delay(WIFI_RETRY_DELAY);
    Serial.print("#");
  }
  return WiFi.status(); // return the WiFi connection status
}


void setup() {
  Serial.begin(115200);
  mySwitch.enableTransmit(4); //D2

  if (init_wifi() == WL_CONNECTED)
  {
    Serial.print("Connetted to ");
    Serial.print(wifi_ssid);
    Serial.print("--- IP: ");
    Serial.println(WiFi.localIP());
  }
  else
  {
    Serial.print("Error connecting to: ");
    Serial.println(wifi_ssid);
  }

  server.begin();
  Serial.println("Server started");
}

void loop() {
  WiFiClient client = server.available();
  if (!client)
  {
    return;
  }
  Serial.println("Waiting for new client");
  while (!client.available())
  {
    delay(1);
  }
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();

// RC CODE from RC-SwitchReciver Demo
// Received 467713 / 24bit Protocol: 1 // on-off
// Received 467721 / 24bit Protocol: 1 // 25%
// Received 467720 / 24bit Protocol: 1 // 50%
// Received 467719 / 24bit Protocol: 1 // 100%
// Received 467718 / 24bit Protocol: 1 // +
// Received 467717 / 24bit Protocol: 1 // -

  // on-off
  if (request.indexOf("/power") != -1)
  {
    mySwitch.send(467718, 24);
    mySwitch.send(467713, 24);
    delay(rc_delay);
  }
  // down
  if (request.indexOf("/down") != -1)
  {
    mySwitch.send(467718, 24);
    delay(rc_delay);
  }
  // up
  if (request.indexOf("/up") != -1)
  {
    mySwitch.send(467717, 24);
    delay(rc_delay);
  }
  // 25%
  if (request.indexOf("/25") != -1)
  {
    mySwitch.send(467721, 24);
    delay(rc_delay);
  }
  // 50%
  if (request.indexOf("/50") != -1)
  {
    mySwitch.send(467720, 24);
    delay(rc_delay);
  }
  // 100%
  if (request.indexOf("/100") != -1)
  {
    mySwitch.send(467719, 24);
    delay(rc_delay);
  }

  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");

}