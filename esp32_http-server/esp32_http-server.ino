#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "PointerToWifi";
const char* password = "PzxdpDbc";

WebServer server(80);

void handleRoot() {
  server.send(200, "text/plain", "Smart Plug is online.");
}

void handleBatteryPost() {
  if (server.method() != HTTP_POST || !server.hasArg("plain")) {
    server.send(400, "text/plain", "Bad Request");
    return;
  }

  String body = server.arg("plain");
  int percent = body.toInt();

  Serial.printf("Received battery percent: %d%%\n", percent);

  if (percent >= 80) {
    digitalWrite(LED_BUILTIN, LOW);  // Stop charging
    Serial.println("Relay OFF\n");
  } else {
    digitalWrite(LED_BUILTIN, HIGH); // Start charging
    Serial.println("Relay ON\n");
  }

  server.send(200, "text/plain", "OK\n");
}


void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected to Wi-Fi");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW); // Default OFF for safety

  server.on("/", HTTP_GET, handleRoot);
  server.on("/battery", HTTP_POST, handleBatteryPost);

  server.begin();
  Serial.println("HTTP server started\n");
}

void loop() {
  server.handleClient();
}
