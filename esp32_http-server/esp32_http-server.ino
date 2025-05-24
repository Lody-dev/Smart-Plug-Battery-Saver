#include <WiFi.h>
#include <WebServer.h>

// === CONFIG ===
const char* ssid = "PointerToWifi";
const char* password = "PzxdpDbc";

const unsigned long HEARTBEAT_TIMEOUT = 2 * 60 * 1000; // 2 min in ms
unsigned long lastUpdateTime = 0;
bool relayIsOn = false;       // track relay/LED state
bool timeoutTriggered = false; // track if timeout shutoff happened

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
    Serial.println("%");

    lastUpdateTime = millis();

    // Reset timeout flag because device is responsive now
    timeoutTriggered = false;

    if (percent >= 80) {
      if (relayIsOn) {
        turnRelayOff();
        relayIsOn = false;
      }
    } else {
      if (!relayIsOn) {
        turnRelayOn();
        relayIsOn = true;
      }
    }

    server.send(200, "text/plain", "OK");
  } else {
    server.send(400, "text/plain", "Bad Request");
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

  if (!timeoutTriggered && now - lastUpdateTime > HEARTBEAT_TIMEOUT) {
    // Timeout occurred and we haven't switched off yet
    if (relayIsOn) {
      turnRelayOff();
      relayIsOn = false;
    }
    timeoutTriggered = true;  // mark that we've done the shutoff
    Serial.println("Heartbeat timeout - relay turned off");
  }
}