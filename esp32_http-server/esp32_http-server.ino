#include <WiFi.h>
#include <WebServer.h>

// === CONFIG ===
const char* ssid = "PointerToWifi";
const char* password = "PzxdpDbc";

const unsigned long HEARTBEAT_TIMEOUT = 2 * 60 * 1000; // 2 min in ms
unsigned long lastUpdateTime = 0;

WebServer server(80);

// === RELEY CONTROL ===
void turnRelayOn() {
  digitalWrite(LED_BUILTIN, HIGH); // LED ON = Charging Allowed
  Serial.println("Charging");
}

void turnRelayOff() {
  digitalWrite(LED_BUILTIN, LOW);  // LED OFF = Charging Cut
  Serial.println("Stopped Charging");
}

void handleRoot() {
  server.send(200, "text/plain", "Smart Plug is online.");
}

// === HANDLE INCOMING BATTERY DATA ===
void handleBatteryPost() {
  if (server.hasArg("plain")) {
    String body = server.arg("plain");
    int percent = body.toInt();

    Serial.print("Battery: ");
    Serial.print(percent);
    Serial.println("%\n");

    lastUpdateTime = millis();

    if (percent >= 80) {
      turnRelayOff();
    } else {
      turnRelayOn();
    }

    server.send(200, "text/plain", "OK\n");
  } else {
    server.send(400, "text/plain", "Bad Request\n");
  }
}

// === SETUP ===
void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  turnRelayOff(); // start OFF

  // WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // HTTP server
  server.on("/", HTTP_GET, handleRoot);
  server.on("/battery", HTTP_POST, handleBatteryPost);
  server.begin();
  Serial.println("HTTP server started");
}

// === LOOP ===
void loop() {
  server.handleClient();
  unsigned long now = millis();
  
  if (now - lastUpdateTime > HEARTBEAT_TIMEOUT) {
    turnRelayOff(); // fallback shutoff
  }
}
