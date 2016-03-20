
#define SSID "YOUR WIFI NETWORK NAME"
#define PASS "YOUR WIFI NETWOTK PASS"

const int ledPin =  13;

#define BUFFER_SIZE 128     //Tampon boyutunu belirler
char buffer[BUFFER_SIZE];   //Tamponu oluşturduk
int ledState = LOW;
int led = 14;
char a = 'sadasda';


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial3.begin(115200);
  pinMode(2, OUTPUT);
  pinMode(ledPin, OUTPUT);
  Serial.println("Setup Begin!");
  delay(1000);
  digitalWrite(2, HIGH);

  sendData("AT+RST\r\n", 2000, false); // reset module
  sendData("AT+CWMODE=1\r\n", 1000, true); // configure as access point
  sendData("AT+CWLAP\r\n", 2000, true);
  connectWiFi(SSID, PASS);
  sendData("AT+CIFSR\r\n", 1000, true); // get ip address
  sendData("AT+CIPMUX=1\r\n", 1000, true); // configure for multiple connections
  sendData("AT+CIPSERVER=1,3000\r\n", 1000, true);
}

void loop() {
  // put your main code here, to run repeatedly:

  if (Serial3.available() > 0)
  {

    int ch_id, packet_len;
    char *pb;
    Serial3.readBytesUntil('\n', buffer, BUFFER_SIZE);    //Tampondaki verileri okuduk

    if (strncmp(buffer, "+IPD,", 5) == 0)
    {
      sscanf(buffer + 5, "%d,%d", &ch_id, &packet_len);

      if (packet_len > 0)
      {
        pb = buffer + 5;
        while (*pb != ':') pb++;
        pb++;
        if (strncmp(pb, "GET /?led", 8) == 0) {

          Serial.print(millis());
          Serial.print(" : ");
          Serial.println(buffer);
          Serial.print( "get led from ch :" );
          Serial.println(ch_id);

          delay(100);
          clearSerialBuffer();

          if (ledState == LOW)
            ledState = HIGH;
          else
            ledState = LOW;
          digitalWrite(ledPin, ledState);

          homepage(ch_id);
        } else if (strncmp(pb, "GET / ", 6) == 0) {
          Serial.print(millis());
          Serial.print(" : ");
          Serial.println(buffer);
          Serial.print( "get Status from ch:" );
          Serial.println(ch_id);

          delay(100);
          clearSerialBuffer();

          homepage(ch_id);
        }
      }
    }
  }
}

void homepage(int ch_id) {
  String Header;        //Http header' ı oluşturduk

  Header =  "HTTP/1.1 200 OK\r\n";
  Header += "Content-Type: text/html\r\n";
  Header += "Connection: close\r\n";
  //Header += "Refresh: 5\r\n";

  String Content;     //Sayfamızın içeriği
  Content = "<center><h1>Hello World!</h1></center><hr><center><h2>Oğuzhan Başer</h2></center><hr>Led State: D";
  Content += String(ledState);
  Content += "<br><a href=\"/?led\"><input type=\"button\" value=\"Led Durum\"></a>";

  Header += "Content-Length: ";
  Header += (int)(Content.length());
  Header += "\r\n\r\n";


  Serial3.print("AT+CIPSEND=");   //Sayfayı belirttiğimiz AT komutu CH_ID ve içerik uzunluğu
  Serial3.print(ch_id);
  Serial3.print(",");
  Serial3.println(Header.length() + Content.length());
  delay(10);

  //içeriği gönderdik
  if (Serial3.find(">")) {
    Serial3.print(Header);
    Serial3.print(Content);
    delay(10);
  }
}

void clearSerialBuffer(void) {
  while ( Serial3.available() > 0 ) {
    Serial3.read();
  }
}

String sendData(String command, const int timeout, boolean debug)
{
  String response = "";     //server' ın bize vereceği cevap

  Serial3.print(command); // server a gönderdiğimiz komut

  long int time = millis();     //timeout ın ilk değeri

  while ( (time + timeout) > millis())
  {
    while (Serial3.available())
    {

      // The esp has data so display its output to the serial window
      char c = Serial3.read(); // read the next character.
      response += c;
    }
  }

  if (debug)      //geribesleme istiyorsak görüntüledik
  {
    Serial.print(response);
  }

  return response;    //cevabı geri döndürdük
}

void connectWiFi(String NetworkSSID, String NetworkPASS) {
  String cmd = "AT+CWJAP=\"";
  cmd += NetworkSSID;
  cmd += "\",\"";
  cmd += NetworkPASS;
  cmd += "\"";

  sendData(cmd, 2000, true);
}
