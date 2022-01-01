#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>

#define CHANNEL 0

typedef struct sens_message
{
  int temp;
  int hum;
  float volt;
} struct_send_message;

// data structure for sending data
typedef struct led_message
{
  bool led_state;
  char message[20];
} struct_led_message;

struct_send_message mySensData;
struct_led_message myLEDData;

bool slaveFound = 0, remoteLEDState = 0;

unsigned long lastTime = 0, lastSendTime = 0;

const uint8_t peerLEDPin = 12;
const uint8_t sendLEDPin = 14;
const uint8_t recvLEDPin = 27;

char sendChar = 0;

esp_now_peer_info_t slave;

// funciton for scanning slave devices
void ScanForSlave()
{
  // get nearby wifi devices number 
  int8_t scanResults = WiFi.scanNetworks();

  // clear slave struct object
  memset(&slave, 0, sizeof(slave));

  // check any slave device found
  if (scanResults == 0)
  {
    Serial.println("No WiFi devices in AP Mode found");
  }
  else
  {
    Serial.print("Found ");
    Serial.print(scanResults);
    Serial.println(" devices ");

    // show found devices informations
    for (int i = 0; i < scanResults; ++i)
    {
      // Get SSID and RSSI for each device found
      String SSID = WiFi.SSID(i);
      int32_t RSSI = WiFi.RSSI(i);
      String BSSIDstr = WiFi.BSSIDstr(i);

      // check found device name is start with "Slave"
      if (SSID.indexOf("Slave") == 0)
      {
        // Show Slave devices informations
        Serial.println("Found a Slave.");
        Serial.print(i + 1);
        Serial.print(": ");
        Serial.print(SSID);
        Serial.print(" [");
        Serial.print(BSSIDstr);
        Serial.print("]");
        Serial.print(" (");
        Serial.print(RSSI);
        Serial.print(")");
        Serial.println("");
        // Get BSSID => Mac Address of the Slave
        int mac[6];
        if (6 == sscanf(BSSIDstr.c_str(), "%x:%x:%x:%x:%x:%x", &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]))
        {
          for (int ii = 0; ii < 6; ++ii)
          {
            slave.peer_addr[ii] = (uint8_t)mac[ii];
          }
        }

        slave.channel = CHANNEL; // pick a channel
        slave.encrypt = 0;       // no encryption

        slaveFound = 1;
        digitalWrite(peerLEDPin, HIGH);   
      }
    }
  }
}

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
    // Retry InitESPNow, add a counte and then restart?
    // InitESPNow();
    // or Simply Restart
    ESP.restart();
  }
}

// callback when data is sent from Master to Slave
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print("Last Packet Sent to: ");
  Serial.println(macStr);

  if (status == ESP_NOW_SEND_SUCCESS)
  {
    Serial.println("Data Successfully Sent");
    digitalWrite(sendLEDPin, LOW);
  }
  else
  {
    Serial.println("Data Sending Fail");
  }
}

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
{
  memcpy(&mySensData, incomingData, sizeof(mySensData));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("Temperature: ");
  Serial.println(mySensData.temp);
  Serial.print("Humidty: ");
  Serial.println(mySensData.hum);
  Serial.print("Voltage: ");
  Serial.println(mySensData.volt);
  Serial.println();
  digitalWrite(recvLEDPin, !digitalRead(recvLEDPin));
}

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);

  // make esp32 station device
  WiFi.mode(WIFI_STA);

  pinMode(peerLEDPin, OUTPUT);
  pinMode(sendLEDPin, OUTPUT);
  pinMode(recvLEDPin, OUTPUT);

  Serial.print("STA MAC: ");
  Serial.println(WiFi.macAddress());

  // ESPNOW initialization function
  InitESPNow();

  // scan nearby slave devices 
  do
  {
    ScanForSlave();
    delay(2000);

    // if no slave is found, restart the scan
  } while (slaveFound == 0);

  // Add peer
  if (esp_now_add_peer(&slave) != ESP_OK)
  {
    Serial.println("Failed to add peer");
    return;
  }

  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);
}

void loop()
{
  if (millis() - lastTime > 1000)
  {
    digitalWrite(sendLEDPin, HIGH);
    sprintf(myLEDData.message, "Hello World %d", sendChar++);
    myLEDData.led_state = remoteLEDState;
    remoteLEDState = !remoteLEDState;

    esp_now_send(slave.peer_addr, (uint8_t *)&myLEDData, sizeof(myLEDData));
    lastTime = millis();
  }
}