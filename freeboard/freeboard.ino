//freeboard Arduino

#include "ESP8266.h"
#include <SoftwareSerial.h>
#include <DHT.h>

#define WLAN_SSID "SSID"          //WiFi SSID
#define WLAN_PASS "PASS"          //WiFi Password
#define WLAN_SECURITY   WLAN_SEC_WPA2

#define DHTPIN 7
#define DHTTYPE DHT11

#define HOST_NAME   "www.dweet.io"
#define HOST_PORT   (3000)

#define thing_name  "thingName"   //cihaza verdiğiniz nesnenin ismi

#define ONBOARDLED 13

SoftwareSerial espSerial(2, 3);
ESP8266 wifi(espSerial);
DHT dht(DHTPIN, DHTTYPE);

unsigned long lastTime = 0, lastTime2 = 0;
String sendData = "", valueStr = "";
uint32_t ip;

void setup(void)
{
  espSerial.begin(38400);
  Serial.begin(38400);
  Serial.print("setup begin\r\n");

  pinMode(ONBOARDLED, OUTPUT);

  dht.begin();

  if (wifi.setOprToStationSoftAP()) {
    Serial.print("to station + softap ok\r\n");
  }
  else {
    Serial.print("to station + softap err\r\n");
    while (1);
  }

  if (wifi.joinAP(WLAN_SSID, WLAN_PASS)) {
    Serial.print("Join AP success\r\n");
    Serial.print("IP:");
    Serial.println(wifi.getLocalIP().c_str());
  }
  else {
    Serial.print("Join AP failure\r\n");
    while (1);
  }

  if (wifi.disableMUX()) {
    Serial.print("single ok\r\n");
  }
  else {
    Serial.print("single err\r\n");
    while (1);
  }

  Serial.print("setup end\r\n");

  Serial.println("Connecting dweet.io..");
  if (wifi.createTCP(HOST_NAME, 80))
  {
    Serial.println("connect OK");
  } else {
    Serial.println("connecting failure!");
    while (1);
  }
}

void loop(void)
{

  if (millis() - lastTime > 2000)
  {
    digitalWrite(ONBOARDLED, HIGH);
    float h = dht.readHumidity();
    float t = dht.readTemperature();

    if (isnan(h) || isnan(t)) {
      Serial.println("Failed to read from DHT sensor!");
    }
    else {
      valueStr = "?tempature=" + String(t) + "&humidty=" + String(h);
    }
    lastTime = millis();
    digitalWrite(ONBOARDLED, LOW);
  }

  if (millis() - lastTime2 > 10000)
  {
    Serial.println("Connecting dweet.io..");
    if (wifi.createTCP(HOST_NAME, 80))
    {
      Serial.println("connect OK");
    } else {
      Serial.println("connecting failure!");
      while (1);
    }

    String content = " HTTP/1.1\r\nHost: dweet.io\r\nConnection: close\r\n\r\n";

    char sendArray[128];
    sendData = "GET /dweet/for/" + String(thing_name) + valueStr + content;
    sendData.toCharArray(sendArray, 128);
    Serial.println(sendData);

    wifi.send((const uint8_t*)sendArray, strlen(sendArray));

    uint8_t myBuffer[128] = { 0 };
    uint32_t len = wifi.recv(myBuffer, sizeof(myBuffer), 10000);

    lastTime2 = millis();
  }
}

