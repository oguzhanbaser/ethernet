/*
  Web Server

 A simple web server that shows the value of the analog input pins.
 using an Arduino Wiznet Ethernet shield.

 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13
 * Analog inputs attached to pins A0 through A5 (optional)

 created 18 Dec 2009
 by David A. Mellis
 modified 9 Apr 2012
 by Tom Igoe
 modified 02 Sept 2015
 by Arturo Guadalupi
 
 */

#include <SPI.h>
#include <Ethernet.h>

#include "index.h"

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
    0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

IPAddress ip(192, 168, 1, 177);

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(80);

int csPin = 8;      // ethernet module cs pin

int ledPin = 2;
int redPin = 3, greenPin = 5, bluePin = 6;

void setup()
{
    Ethernet.init(csPin); // ethernet module cs pin 

    pinMode(ledPin, OUTPUT);
    pinMode(redPin, OUTPUT);
    pinMode(greenPin, OUTPUT);
    pinMode(bluePin, OUTPUT);

    // Open serial communications and wait for port to open:
    Serial.begin(115200);

    Serial.println("Ethernet WebServer Example");

    // start the Ethernet connection and the server:
    Ethernet.begin(mac, ip);

    // Check for Ethernet hardware present
    if (Ethernet.hardwareStatus() == EthernetNoHardware)
    {
        Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
        while (true)
        {
            delay(1); // do nothing, no point running without Ethernet hardware
        }
    }
    if (Ethernet.linkStatus() == LinkOFF)
    {
        Serial.println("Ethernet cable is not connected.");
    }

    // start the server
    server.begin();
    Serial.print("server is at ");
    Serial.println(Ethernet.localIP());
}

void loop()
{
    // listen for incoming clients
    EthernetClient client = server.available();
    if (client)
    {
        String readString = "";

        while (client.connected())
        {
            if (client.available())
            {
                char c = client.read();
                readString += c;

                // if you've gotten to the end of the line (received a newline
                // character) and the line is blank, the http request has ended,
                // so you can send a reply
                if (c == '\n')
                {
                    Serial.println(readString);
                    if (readString.indexOf("setLED") > 0)
                    {
                        send_header(client);
                        if (readString.indexOf("LEDstate=1") > 0)
                        {
                            digitalWrite(ledPin, HIGH);
                            client.println("ON");
                        }
                        else if (readString.indexOf("LEDstate=0") > 0)
                        {
                            digitalWrite(ledPin, LOW);
                            client.println("OFF");
                        }
                        break;
                    }
                    else if (readString.indexOf("readADC") > 0)
                    {
                        send_header(client);
                        client.println(analogRead(A0));
                        break;
                    }else if(readString.indexOf("setRGB") > 0)
                    {
                        int indx = readString.indexOf("=");
                        char rgbArr[7]; rgbArr[6] = '\0';
                        for(int i = 0; i < 6; i++)
                        {
                            rgbArr[i] = readString.charAt(i + indx + 1);
                        }
                        long rgbInt = strtol(rgbArr, 0, 16);
                        uint8_t rVal = (uint8_t)((rgbInt & 0xFF0000) >> 16);
                        uint8_t gVal = (uint8_t)((rgbInt & 0x00FF00) >> 8);
                        uint8_t bVal = (uint8_t)((rgbInt & 0x0000FF) >> 0);
                        analogWrite(redPin, rVal);
                        analogWrite(greenPin, gVal);
                        analogWrite(bluePin, bVal);
                        
                        send_header(client);
                        client.println("OK");
                        break;
                    }

                    SendHTML(client);
                    break;
                }
            }
        }
        // give the web browser time to receive the data
        delay(1);
        // close the connection:
        client.stop();
        // Serial.println("client disconnected");
    }
}

void send_header(EthernetClient pClient)
{
    for (int i = 0; i < sizeof(header_plain); i++)
    {
        char c = pgm_read_byte(header_plain + i);
        // Serial.print(c);
        pClient.print(c);
    }
}

void SendHTML(EthernetClient pClient)
{
    for (int i = 0; i < sizeof(header_html); i++)
    {
        char c = pgm_read_byte(header_html + i);
        // Serial.print(c);
        pClient.print(c);
    }
    // pClient.println();
    for (int i = 0; i < sizeof(myHtml); i++)
    {
        char c = pgm_read_byte(myHtml + i);
        // Serial.print(c);
        pClient.print(c);
    }
    // pClient.println();
}
