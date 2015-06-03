//A Led attach digital-4 pin
//Other SPI connections are same


#include <EtherCard.h>

static byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };
static byte myip[] = { 192,168,19,171};

byte Ethernet::buffer[500];
BufferFiller bfill;

int ledPin = 4;

char *on = "ON";
char *off = "OFF";

char *statusLabel;    //labelden gelen verileri diziye atadık
char *buttonLabel;    //butondan gelen verileri diziye atadık

bool ledStatus = false;

void setup()
{
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  Serial.begin(9600); 
  if (ether.begin(sizeof Ethernet::buffer, mymac) == 0)
    Serial.println(F("Failed to access Ethernet controller"));
  ether.staticSetup(myip);
}

static word homePage()
{
  bfill = ether.tcpOffset();
  bfill.emit_p(PSTR("HTTP/1.0 200 OK\r\n"
    "Content-Type: text/html\r\n"
    "Pragma: no-cache\r\n"
    "\r\n"
    "<html><head><title>Arduino</title></head>" 
    "<body><form>Led Status: $S<a href=\"/?status=$S\"><input type=\"button\" value=\"$S\"></a>"
    "</form></body></html>"), statusLabel, buttonLabel, buttonLabel);
  return bfill.position();
}

void loop()
{
  word len = ether.packetReceive();
  word pos = ether.packetLoop(len);
  
  if(strstr((char*) Ethernet :: buffer + pos, "GET /?status=ON") != 0)
  {
    Serial.println("LED AC");
    ledStatus = true;
  }
  if(strstr((char*) Ethernet :: buffer + pos, "GET /?status=OFF") != 0)
  {
    Serial.println("LED KAPA");
    ledStatus = false;
  }
  
  if(ledStatus)
  {
    digitalWrite(ledPin, HIGH);
    statusLabel = on;
    buttonLabel = off;
  }else{
    digitalWrite(ledPin, LOW);
    statusLabel = off;
    buttonLabel = on;
  }
  
  if (pos)  // check if valid tcp data is received
    ether.httpServerReply(homePage()); // send web page data
}
