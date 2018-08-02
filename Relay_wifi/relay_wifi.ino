/*
* This sketch demonstrates how to set up a simple HTTP-like server.
* The server will set a GPIO pin depending on the request
* http://server_ip/gpio/0 will set the GPIO2 low,
* http://server_ip/gpio/1 will set the GPIO2 high
* server_ip is the IP address of the ESP8266 module, will be
* printed to Serial when the module is connected.
*/

#include <ESP8266WiFi.h>

const char* ssid = "L.Connected";
const char* password = "connected";
IPAddress arduino_ip ( 192,  168,  1,  207);
IPAddress dns_ip     (  8,  8,   8,   8);
IPAddress gateway_ip ( 192,  168,   1,   1);

IPAddress subnet_mask(255, 255, 255,   0);

// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(80);

void setup() {
Serial.begin(9600);
delay(10);

// prepare GPIO2
pinMode(2, OUTPUT);
digitalWrite(2, 0);

// Connect to WiFi network
Serial.println();
Serial.println();
Serial.print("Connecting to ");
Serial.println(ssid);

WiFi.config(arduino_ip, gateway_ip, subnet_mask);
WiFi.begin(ssid, password);

while (WiFi.status() != WL_CONNECTED) {
delay(500);
Serial.print(".");
}
Serial.println("");
Serial.println("WiFi connected");

// Start the server
server.begin();
Serial.println("Server started");

// Print the IP address
Serial.println(WiFi.localIP());
}

void loop() {
// Check if a client has connected
WiFiClient client = server.available();
if (!client) {
return;
}

// Wait until the client sends some data
Serial.println("new client");
while(!client.available()){
delay(1);
}

// Read the first line of the request
String req = client.readStringUntil('\r');
Serial.println(req);
client.flush();

// Match the request
int val;
if (req.indexOf("/door/0") != -1)
val = 0;
else if (req.indexOf("/door/1") != -1)
val = 1;
else {
Serial.println("invalid request");
client.stop();
return;
}

// Set GPIO2 according to the request
digitalWrite(2, val);
if (val == 1) {
  delay(1000);
  digitalWrite(2, 0);
}

client.flush();

// Prepare the response
  String s = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n{\"IDkey\":\"ESP-1\",\"sensors\":[{\"type\":\"door\",\"value\":";
  s += (val) ? "open":"close";
  s += "}]}\r\n\r\n";
  
// Send the response to the client
client.print(s);
delay(1);
Serial.println("Client disonnected");

// The client will actually be disconnected
// when the function returns and "client" object is detroyed
}
