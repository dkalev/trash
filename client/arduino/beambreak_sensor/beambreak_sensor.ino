#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include "secrets.h"
#include <WiFiUdp.h>

// Set these to run example.
#define FIREBASE_HOST "trash-5388a.firebaseio.com"
#define THRESHOLD 5

String path;

const char* geoHost = "location.services.mozilla.com";
String geoThisPage = "/v1/geolocate?key=";

// NTP
unsigned int localPort = 2390;      // local port to listen for UDP packets

/* Don't hardwire the IP address or we won't get the benefits of the pool.
 *  Lookup the IP address for the host name instead */
//IPAddress timeServer(129, 6, 15, 28); // time.nist.gov NTP server
IPAddress timeServerIP; // time.nist.gov NTP server address
const char* ntpServerName = "time.nist.gov";

const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message

byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets

// A UDP instance to let us send and receive packets over UDP
WiFiUDP udp;

String macToStr(const uint8_t* mac)
{
    String result;
    for (int i = 0; i < 6; ++i) {
        result += String(mac[i], 16);
        if (i < 5)
            result += ':';
    }
    return result;
}

void setup() {
  Serial.begin(115200);
  path = "bins/";
  unsigned char mac[6];
  WiFi.macAddress(mac);
  path += macToStr(mac);
  Serial.print("Firebase path: ");
  Serial.println(path);

  // Geolocation scanning
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  Serial.println("Disconnected");
  
  Serial.println("scan start");
  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0)
    Serial.println("no networks found");
  else
  {
    Serial.print(n);
    Serial.println(" networks found...");
  }
  String jsonString = "{\n";
  jsonString +="\"wifiAccessPoints\": [\n";
  for (int j = 0; j < n; ++j)
  {
    jsonString +="{\n";
    jsonString +="\"macAddress\" : \"";    
    jsonString +=(WiFi.BSSIDstr(j));
    jsonString +="\",\n";
    jsonString +="\"signalStrength\": ";     
    jsonString +=WiFi.RSSI(j);
    jsonString +="\n";
    if(j<n-1)
    {
    jsonString +="},\n";
    }
    else
    {
    jsonString +="}\n";  
    }
  }
  jsonString +=("]\n");
  jsonString +=("}\n");
  Serial.print(jsonString);

  
  // connect to wifi.
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());
  
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  //Geolocaion sending
  WiFiClientSecure client;
  DynamicJsonBuffer jsonBuffer;
  Serial.print("Requesting URL: ");
  Serial.println("https://" + (String)geoHost + geoThisPage + geoKey);
  Serial.println(" ");
  if (client.connect(geoHost, 443)) {
    Serial.println("Connected");    
    client.println("POST " + geoThisPage + geoKey + " HTTP/1.1");
    client.println("Host: "+ (String)geoHost);
    client.println("Connection: close");
    client.println("Content-Type: application/json");
    client.println("User-Agent: Arduino/1.0");
    client.print("Content-Length: ");
    client.println(jsonString.length());    
    client.println();
    client.print(jsonString);  
    delay(500);
  }

  //Read and parse all the lines of the reply from server          
  while (client.available()) {
    String line = client.readStringUntil('\r');
    JsonObject& root = jsonBuffer.parseObject(line);
    if(root.success()){
    double latitude    = root["location"]["lat"];
    double longitude   = root["location"]["lng"];
    Firebase.setFloat(path + "/lat", latitude);
    Firebase.setFloat(path + "/long", longitude);
    Firebase.set(path + "/geolocation", root);
    Serial.print("Latitude = ");
    Serial.println(latitude,6);
    Serial.print("Longitude = ");
    Serial.println(longitude,6);
    }
  }
  Serial.println("Starting UDP");
  udp.begin(localPort);
  Serial.print("Local port: ");
  Serial.println(udp.localPort());
  pinMode(LED_BUILTIN, OUTPUT);
}

// send an NTP request to the time server at the given address
unsigned long sendNTPpacket(IPAddress& address)
{
  Serial.println("sending NTP packet...");
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  udp.beginPacket(address, 123); //NTP requests are to port 123
  udp.write(packetBuffer, NTP_PACKET_SIZE);
  udp.endPacket();
}

unsigned long getEpochTime()
{
  Serial.println("Getting time");
  //get a random server from the pool
  WiFi.hostByName(ntpServerName, timeServerIP); 

  sendNTPpacket(timeServerIP); // send an NTP packet to a time server
  // wait to see if a reply is available
  delay(1000);
  
  int cb = udp.parsePacket();
  int retries = 10;
  while (!cb) {
    if(retries <= 0){
      Serial.println("fail");
      return 0;
    }
    Serial.println("no packet yet");
    delay(1000);
    cb = udp.parsePacket();
    retries--;
  }
  Serial.print("packet received, length=");
  Serial.println(cb);
  // We've received a packet, read the data from it
  udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

  //the timestamp starts at byte 40 of the received packet and is four bytes,
  // or two words, long. First, esxtract the two words:

  unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
  unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
  // combine the four bytes (two words) into a long integer
  // this is NTP time (seconds since Jan 1 1900):
  unsigned long secsSince1900 = highWord << 16 | lowWord;
  Serial.print("Seconds since Jan 1 1900 = " );
  Serial.println(secsSince1900);

  // now convert NTP time into everyday time:
  // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
  const unsigned long seventyYears = 2208988800UL;
  // subtract seventy years:
  unsigned long epoch = secsSince1900 - seventyYears;
  // print Unix time:
  return epoch;
}

int state = -1;
int nextState = -1;
int count = 0;

int classify(int reading){
    if(reading > 40){
        digitalWrite(LED_BUILTIN, HIGH);
        return 1;
    } else {
        digitalWrite(LED_BUILTIN, LOW);
        return 0;
    }
}

void heartbeat(int state){
    Serial.print("New state: ");
    Serial.println(state);
    Firebase.setBool(path + "/isEmpty", state);
    if (Firebase.failed()) {
        Serial.print("Heartbeat failed: ");
        Serial.println(Firebase.error());  
    } else {
        Serial.println("Successful heartbeat");
        Firebase.setInt(path + "/lastEvent", getEpochTime());
        if (Firebase.failed()) {
            Serial.print("Timestamp update failed: ");
            Serial.println(Firebase.error());  
        } else {
            Serial.println("Successful timestamp update");
        }
    }
}

void loop() {
    int classification = classify(analogRead(A0));
    if(classification == nextState){
        count++;
        if(count >= THRESHOLD && nextState != state){
            heartbeat(nextState);
            state = nextState;
        }
    } else {
        nextState = classification;
        count = 0;
    }
    delay(500);
}
