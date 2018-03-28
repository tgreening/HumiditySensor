#include <ESP8266WiFi.h>

String apiKey = "";
const char* ssid     = "";
const char* password = "";
const int DELAY_SECONDS = 3600000;
IPAddress server(184, 106, 153, 149);


void setup() {
  Serial.begin(115200);
 Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
     would try to act as both a client and an access-point and could cause
     network-issues with your other WiFi-devices on your WiFi-network. */
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  int humidityRaw = analogRead(A0);
  int humidityReal = map(humidityRaw, 1023, 0, 0, 100);
  Serial.println(humidityReal);
  sendToTS(humidityReal);
  delay(DELAY_SECONDS);
}

void sendToTS(int humidityReal) {
  delay(100);
  yield();
  WiFiClient client;

  if (client.connect(server, 80)) { // use ip 184.106.153.149 or api.thingspeak.com
    Serial.println("WiFi Client connected ");

    String postStr = apiKey;
    postStr += "&field1=";
    postStr += String(humidityReal);
    postStr += "\r\n\r\n";

    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);

    while (client.available()) {
      String line = client.readStringUntil('\r');
      Serial.print(line);
    }
    delay(100);
  } else {
    Serial.println("Couldn't connect....");
  }
  client.stop();
}

