//freeboard Arduino

#include "ESP8266.h"
#include <SoftwareSerial.h>
#include <DHT.h>

#define WLAN_SSID "SSID"          //WiFi SSID
#define WLAN_PASS "PASS"          //WiFi Password
#define WLAN_SECURITY   WLAN_SEC_WPA2

#define DHTPIN 7                //dht11 ayarları
#define DHTTYPE DHT11

#define HOST_NAME   "www.dweet.io"
#define HOST_PORT   (80)

#define thing_name  "thingName"   //cihaza verdiğiniz nesnenin ismi

#define ONBOARDLED 13

SoftwareSerial espSerial(2, 3);   //esp8266 
ESP8266 wifi(espSerial);        
DHT dht(DHTPIN, DHTTYPE);          //dht11

unsigned long lastTime = 0, lastTime2 = 0;
String sendData = "", valueStr = "";

void setup(void)
{
  espSerial.begin(38400);               //esp8266 haberleşmesini başlattık
  Serial.begin(38400);                  //seri port haberleşmesini başlattık
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

  //wifi bağlantısı yaptık
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
}

void loop(void)
{

  //DHT11 verilerini 2 sn de bir okuduk
  if (millis() - lastTime > 2000)
  {
    digitalWrite(ONBOARDLED, HIGH);   //okuma işlemi başladı
    float h = dht.readHumidity();
    float t = dht.readTemperature();

    if (isnan(h) || isnan(t)) {
      Serial.println("Failed to read from DHT sensor!");
    }
    else {
      //sensörden alınan değerleri gönderilecek string e ekledik
      valueStr = "?tempature=" + String(t) + "&humidty=" + String(h);
    }
    lastTime = millis();
    digitalWrite(ONBOARDLED, LOW);    //okuma işlemi bitti
  }

  //her 10 sn de bir dweet.io ya tcp soket ile bağlandık
  if (millis() - lastTime2 > 10000)
  {
    Serial.println("Connecting dweet.io..");
    if (wifi.createTCP(HOST_NAME, HOST_PORT))
    {
      Serial.println("connect OK");
    } else {
      Serial.println("connecting failure!");
      while (1);
    }

    char sendArray[128];

    //içeriği oluşturduk
    String content = " HTTP/1.1\r\nHost: dweet.io\r\nConnection: close\r\n\r\n";

    //http get metodunu oluştrduk.
    //content ile birleştirip tcp server' a göndereeğiz
    sendData = "GET /dweet/for/" + String(thing_name) + valueStr + content;
    sendData.toCharArray(sendArray, 128);         //string veriyi karakter dizisine çevirdik
    Serial.println(sendData);                     //gönderilen veriyi gördük

    wifi.send((const uint8_t*)sendArray, strlen(sendArray));    //tcp server a gönder

    uint8_t myBuffer[128] = { 0 };
    uint32_t len = wifi.recv(myBuffer, sizeof(myBuffer), 10000);

    lastTime2 = millis();
  }
}

