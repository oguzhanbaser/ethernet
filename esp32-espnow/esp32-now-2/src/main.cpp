/**
   ESPNOW - Basic communication - Slave
   Date: 26th September 2017
   Author: Arvind Ravulavaru <https://github.com/arvindr21>
   Purpose: ESPNow Communication between a Master ESP32 and a Slave ESP32
   Description: This sketch consists of the code for the Slave module.
   Resources: (A bit outdated)
   a. https://espressif.com/sites/default/files/documentation/esp-now_user_guide_en.pdf
   b. http://www.esploradores.com/practica-6-conexion-esp-now/

   << This Device Slave >>

   Flow: Master
   Step 1 : ESPNow Init on Master and set it in STA mode
   Step 2 : Start scanning for Slave ESP32 (we have added a prefix of `slave` to the SSID of slave for an easy setup)
   Step 3 : Once found, add Slave as peer
   Step 4 : Register for send callback
   Step 5 : Start Transmitting data from Master to Slave

   Flow: Slave
   Step 1 : ESPNow Init on Slave
   Step 2 : Update the SSID of Slave with a prefix of `slave`
   Step 3 : Set Slave in AP mode
   Step 4 : Register for receive callback and wait for data
   Step 5 : Once data arrives, print it in the serial monitor

   Note: Master and Slave have been defined to easily understand the setup.
         Based on the ESPNOW API, there is no concept of Master and Slave.
         Any devices can act as master or salve.
*/

#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>

#include <Adafruit_Sensor.h>
#include <DHT.h>

#define DHTPIN 14
#define DHTTYPE DHT11

uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

#define CHANNEL 0

typedef struct sens_message
{
  int temp;
  int hum;
  float volt;
} struct_send_message;

typedef struct led_message
{
  bool led_state;
  char message[20];
} struct_led_message;

// Create a struct_message called myData
struct_send_message mySensData;
struct_led_message myLedData;

esp_now_peer_num_t peerNum;
esp_now_peer_info_t peerInfo = {0};

DHT dht(DHTPIN, DHTTYPE);

const uint8_t peerLEDPin = 12;
const uint8_t remoteLEDPin = 14;
const uint8_t sendLEDPin = 27;
const uint8_t potPin = 34; 

bool isConnected = false;
unsigned long lastTimeSend = 0, lastTimeRead = 0;

esp_now_peer_info_t peer;

void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len);

// Init ESP Now with fallback
void InitESPNow()
{
  WiFi.disconnect();
  if (esp_now_init() == ESP_OK)
  {
    Serial.println("ESPNow Init Success");
  }
  else
  {
    Serial.println("ESPNow Init Failed");
    ESP.restart();
  }
}

void setup()
{
  dht.begin();
  pinMode(peerLEDPin, OUTPUT);
  pinMode(remoteLEDPin, OUTPUT);
  pinMode(sendLEDPin, OUTPUT);

  Serial.begin(115200);
  Serial.println("ESPNow/Basic/Slave Example");
  //Set device in AP and STA mode to begin with
  WiFi.mode(WIFI_MODE_APSTA);
  // configure device AP mode
  const char *SSID = "Slave_1";
  bool result = WiFi.softAP(SSID, "Slave_1_Password", CHANNEL, 0);
  if (!result)
  {
    Serial.println("AP Config failed.");
  }
  else
  {
    Serial.println("AP Config Success. Broadcasting with AP: " + String(SSID));
  }
  // This is the mac address of the Slave in AP Mode
  Serial.print("AP MAC: ");
  Serial.println(WiFi.softAPmacAddress());
  // Init ESPNow with a fallback logic
  InitESPNow();
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info.
  esp_now_register_recv_cb(OnDataRecv);

}

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
{
  // copy received data into struct 
  memcpy(&myLedData, incomingData, sizeof(myLedData));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("Char: ");
  Serial.println(myLedData.message);
  Serial.print("Bool: ");
  Serial.println(myLedData.led_state);
  Serial.println();

  // change led status from received data
  digitalWrite(remoteLEDPin, myLedData.led_state);

  // check already paired with received MAC address
  if (!esp_now_is_peer_exist(mac))
  {
    // clear peer info
    memset(&peer, 0, sizeof(peer));

    // copy received MAC address into peer info
    for (int ii = 0; ii < 6; ++ii)
    {
      peer.peer_addr[ii] = (uint8_t)mac[ii];
    }
    peer.channel = CHANNEL; // pick a channel
    peer.encrypt = 0;       // no encryption

    if(esp_now_add_peer(&peer) == ESP_OK)
    {
      Serial.println("Peer added successfully");
    }
    else
    {
      Serial.println("Failed to add peer");
    }
  }
}

void loop()
{
  //get peered devices count
  esp_now_get_peer_num(&peerNum);

  // read sensor values every 1 seconds async
  if (millis() - lastTimeRead > 1000)
  {
    mySensData.hum = dht.readHumidity();
    mySensData.temp = dht.readTemperature();
    mySensData.volt = analogRead(potPin) / 4096.0 * 3.3;
    lastTimeRead = millis();
  }

  // if peer number greater then 0, get peer data and save it to peerInfo
  if (peerNum.total_num > 0)
  {
    if (isConnected == false)
    {
      esp_now_fetch_peer(1, &peerInfo);
      Serial.println("Slave: Connected to Master");
      Serial.println();
      digitalWrite(peerLEDPin, HIGH);

      delay(5000);

      isConnected = true;
    }
  }
  else
  {
    digitalWrite(peerLEDPin, LOW);
    isConnected = false;
  }

  // if conencted to peer send data to peer every 2 seconds
  if (isConnected)
  {
    if (millis() - lastTimeSend > 2000)
    {
      // send data to peer
      esp_err_t rr = esp_now_send(peerInfo.peer_addr, (uint8_t *)&mySensData, sizeof(mySensData));
      
      // check data sent successfully or not
      if (rr == ESP_OK)
      {
        Serial.println("Slave: Data sent successfully");
        digitalWrite(sendLEDPin, !digitalRead(sendLEDPin));
      }
      else
      {
        Serial.println("Slave: Failed to send data");
      }

      lastTimeSend = millis();
    }
  }
}