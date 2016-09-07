
#include "ESP8266.h"
#include <SoftwareSerial.h>
#include <DHT.h>

#define SSID "SSID"
#define PASSWORD "PASS"
#define DHTPIN 7
#define DHTTYPE DHT11

#define HOST_NAME   "HOST ADRESS"
#define HOST_PORT   (3000)

#define ledPin1  9
#define ledPin2  10
#define ledPin3  11


SoftwareSerial espSerial(2, 3);
ESP8266 wifi(espSerial);
DHT dht(DHTPIN, DHTTYPE);

unsigned long lastTime = 0;
String sendData = "";

void setup(void)
{
	espSerial.begin(38400);
	Serial.begin(38400);
	Serial.print("setup begin\r\n");

	pinMode(ledPin1, OUTPUT);
	pinMode(ledPin2, OUTPUT);
	pinMode(ledPin3, OUTPUT);

	dht.begin();

	Serial.print("FW Version:");
	Serial.println(wifi.getVersion().c_str());

	if (wifi.setOprToStationSoftAP()) {
		Serial.print("to station + softap ok\r\n");
	}
	else {
		Serial.print("to station + softap err\r\n");
	}

	if (wifi.joinAP(SSID, PASSWORD)) {
		Serial.print("Join AP success\r\n");
		Serial.print("IP:");
		Serial.println(wifi.getLocalIP().c_str());
	}
	else {
		Serial.print("Join AP failure\r\n");
	}

	if (wifi.disableMUX()) {
		Serial.print("single ok\r\n");
	}
	else {
		Serial.print("single err\r\n");
	}

	if (wifi.createTCP(HOST_NAME, HOST_PORT)) {
		Serial.print("create tcp ok\r\n");
	}
	else {
		Serial.print("create tcp err\r\n");
	}

	Serial.print("setup end\r\n");
}

void loop(void)
{
	uint8_t buffer[128] = { 0 };

	if (millis() - lastTime > 2000)
	{
		float h = dht.readHumidity();
		float t = dht.readTemperature();

		if (isnan(h) || isnan(t)) {
			Serial.println("Failed to read from DHT sensor!");
		}
		else {
			sendData = "{ temp : " + String(t) + " , hum : " + String(h) + " }";
			Serial.println(sendData);
		}
		lastTime = millis();
	}

	char hello[40];
	sendData.toCharArray(hello, 40);

	wifi.send((const uint8_t*)hello, strlen(hello));

	uint32_t len = wifi.recv(buffer, sizeof(buffer), 10000);
	if (len > 0) {
		if (strncmp((char*)buffer, "rgb>", 3) == 0) {
			ledParser((char *)buffer);

		}
		/*
		Serial.print("Received:[");
		for (uint32_t i = 0; i < len; i++) {
		Serial.print((char)buffer[i]);
		}
		Serial.print("]\r\n");*/
	}

	/*
	if (wifi.releaseTCP()) {
	Serial.print("release tcp ok\r\n");
	} else {
	Serial.print("release tcp err\r\n");
	}*/

	delay(500);
}

void ledParser(char * str)
{
	String s1(str);

	int num1 = s1.indexOf("red");
	int num2 = s1.indexOf("green");
	int num3 = s1.indexOf("blue");
	int num4 = s1.indexOf("#");

	int val1 = s1.substring(num1 + 1 + 3, num2).toInt();
	int val2 = s1.substring(num2 + 1 + 5, num3).toInt();
	int val3 = s1.substring(num3 + 1 + 4, num4).toInt();

	if (val1 > 255) val1 = 255;
	if (val2 > 255) val2 = 255;
	if (val3 > 255) val3 = 255;

	analogWrite(ledPin1, val1);
	analogWrite(ledPin2, val2);
	analogWrite(ledPin3, val3);

	Serial.print(val1); Serial.print("\t");
	Serial.print(val2); Serial.print("\t");
	Serial.println(val3);
}

