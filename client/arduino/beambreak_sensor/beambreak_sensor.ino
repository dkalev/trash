#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include "secrets.h"

// Set these to run example.
#define FIREBASE_HOST "trash-5388a.firebaseio.com"
#define THRESHOLD 5

String path;

const char* geoHost = "location.services.mozilla.com";
String geoThisPage = "/v1/geolocate?key=";

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

}

int state = -1;
int nextState = -1;
int count = 0;

int classify(int reading){
    return reading > 25 ? 1 : 0;
}

void heartbeat(int state){
    Serial.print("New state: ");
    Serial.println(state);
    Firebase.setBool(path + "/isEmpty", state);
    if (Firebase.failed()) {
        Serial.print("Heartbeat failed:");
        Serial.println(Firebase.error());  
        return;
    } else {
        Serial.println("Successful heartbeat");
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
