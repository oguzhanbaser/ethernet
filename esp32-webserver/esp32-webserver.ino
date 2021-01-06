#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include "pages.h"

const char *ssid = "SUPERONLINE_WiFi_7938";
const char *password = "47VF4PXKJKFX";
int adcVal = 0;
int ledPins[] = {2, 4};

const int rPin = 5;
const int gPin = 18;
const int bPin = 19;

const int freq = 5000;
const int ledChannel1 = 0;
const int ledChannel2 = 1;
const int ledChannel3 = 2;
const int resolution = 8;

IPAddress myIpadress;

AsyncWebServer server(80);

// toggle LED callback
void handleLED(AsyncWebServerRequest *request)
{
    String t_state = request->arg("LEDstate"); //Refer  xhttp.open("GET", "setLED?LEDstate="+led, true);
    String t_num = request->arg("ledNum");
    Serial.print(t_state);
    Serial.print("\t");
    Serial.println(t_num);

    digitalWrite(ledPins[t_num.toInt()], t_state.toInt());

    request->send(200, "text/plane", "1"); //Send web page
}

// rgb brightness callback
void handleRGB(AsyncWebServerRequest *request)
{
    // get RGB values from parameters
    String rVal = request->arg("r");
    String gVal = request->arg("g");
    String bVal = request->arg("b");

    // show values on serial monitor
    Serial.print(rVal);
    Serial.print("\t");
    Serial.print(gVal);
    Serial.print("\t");
    Serial.println(bVal);

    // set duty cycle for RGB channels
    ledcWrite(ledChannel1, rVal.toInt());
    ledcWrite(ledChannel2, gVal.toInt());
    ledcWrite(ledChannel3, bVal.toInt());

    // send answer to client
    request->send(200, "text/plane", "1");
}

// adc request callback
void handleADC(AsyncWebServerRequest *request)
{
    String a = String(adcVal);
    request->send(200, "text/plane", String(a));
}

// home page request callback
void handleRoot(AsyncWebServerRequest *request)
{
    Serial.println("Connected");
    request->send(200, "text/html", MAIN_PAGE);
}

// requested page not found callback
void handleNotFound(AsyncWebServerRequest *request)
{
    request->send(404, "text/html", PAGE_404);
}

void setup()
{
    // led pins as a output
    pinMode(ledPins[0], OUTPUT);
    pinMode(ledPins[1], OUTPUT);
    pinMode(rPin, OUTPUT);
    pinMode(gPin, OUTPUT);
    pinMode(bPin, OUTPUT);

    // set required cycles for complete analog digital conversion 
    analogSetCycles(32);

    // set pwm channeles, freqencies and pins
    ledcSetup(ledChannel1, freq, resolution);
    ledcSetup(ledChannel2, freq, resolution);
    ledcSetup(ledChannel3, freq, resolution);
    ledcAttachPin(rPin, ledChannel1);
    ledcAttachPin(gPin, ledChannel2);
    ledcAttachPin(bPin, ledChannel3);

    // start serial communicatin
    Serial.begin(115200);
    Serial.println();
    Serial.println("Configuring access point...");

    // set wifi mode, ssid and password
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    // show wifi connection informaion
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    myIpadress = WiFi.localIP();
    Serial.println(myIpadress);

    // set callback funcitons to answer requests
    server.on("/", HTTP_GET, handleRoot);
    server.on("/readADC", HTTP_GET, handleADC);
    server.on("/setLED", HTTP_GET, handleLED);
    server.on("/setRGB", HTTP_GET, handleRGB);
    server.onNotFound(handleNotFound);

    //start server
    server.begin();
}

void loop()
{
    // read adc value continuously
    adcVal = analogRead(34);
    digitalWrite(15, !digitalRead(15));
    delay(25);
}
