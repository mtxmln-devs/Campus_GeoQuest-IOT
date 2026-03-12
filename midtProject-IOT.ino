#include <WiFi.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>
#include <TinyGPSPlus.h>
#include <HardwareSerial.h>

// ============================================================
// CONFIGURATION BLOCK
// ============================================================
const char* ssid         = "POCO X7 Pro" ;          // phone hotspot name
const char* password     = "laurence";      // phone hotspot password
const char*    ws_host = "esp32-tracker.kiyokodyele.com"; //WebSocket server hostname
const uint16_t ws_port = 443;                             //WSS port (SSL)
const char* device_id    = "ESP32_TresCepas";            // "ESP32_" + team name (no spaces)
const char* device_name  = "TresCepas";                 // team name
const char* avatar_url   = "https://toppng.com/uploads/preview/transparent-png-army-memes-11554020360yneps0x2fy.png"; //  image URL




// GPS and WebSocket objects
TinyGPSPlus gps;
HardwareSerial GPS_Serial(2);   // UART2 = pins 16 (RX) and 17 (TX)
WebSocketsClient webSocket;

// Timing
unsigned long lastSendTime = 0;
const unsigned long SEND_INTERVAL = 3000; // Send every 3 seconds

bool isConnected = false;

// ============================================================
// WebSocket Event Handler
// ============================================================
void webSocketEvent(WStype_t type, uint8_t* payload, size_t length) {
  switch (type) {

    case WStype_CONNECTED:
      Serial.println("WebSocket Connected!");
      isConnected = true;
      registerDevice();   // Announce our device to the server immediately
      break;

    case WStype_DISCONNECTED:
      Serial.println("WebSocket Disconnected. Retrying...");
      isConnected = false;
      break;

    case WStype_TEXT:
      Serial.print("Message from server: ");
      Serial.println((char*)payload);
      break;

    case WStype_ERROR:
      Serial.println("WebSocket Error!");
      break;

    default:
      break;
  }
}

// ============================================================
// Register Device — sent once on connect
// ============================================================
void registerDevice() {
  StaticJsonDocument<512> doc;
  doc["type"]      = "register";
  doc["deviceId"]  = device_id;
  doc["name"]      = device_name;
  doc["avatarUrl"] = avatar_url;
  doc["latitude"]  = 0.0;
  doc["longitude"] = 0.0;

  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer);
  webSocket.sendTXT(jsonBuffer);

  Serial.println("Registration sent:");
  Serial.println(jsonBuffer);
}

// ============================================================
// Send Location Update — sent every 3 seconds
// ============================================================
void sendLocation(double lat, double lng) {
  StaticJsonDocument<512> doc;
  doc["type"]      = "location";
  doc["latitude"]  = lat;
  doc["longitude"] = lng;
  doc["avatarUrl"] = avatar_url;

  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer);
  webSocket.sendTXT(jsonBuffer);

  Serial.print("Location sent → Lat: ");
  Serial.print(lat, 6);
  Serial.print("  Lng: ");
  Serial.println(lng, 6);
}

// ============================================================
// SETUP
// ============================================================
void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\nCampus GeoQuest Starting...");

  // Step 1: Start GPS on UART2 (pins 16 RX, 17 TX)
  GPS_Serial.begin(9600, SERIAL_8N1, 16, 17);
  Serial.println("GPS UART initialized on pins 16/17");

  // Step 2: Connect to WiFi
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    attempts++;
    if (attempts > 40) {
      Serial.println("\nWiFi failed! Check SSID/password. Restarting...");
      ESP.restart();
    }
  }

  Serial.println("\nWiFi Connected!");
  Serial.print("   IP Address: ");
  Serial.println(WiFi.localIP());

  // Step 3: Connect to WebSocket server (SSL/TLS on port 443)
  Serial.println(" Connecting to WebSocket server...");
  webSocket.beginSSL(ws_host, ws_port, "/");
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(5000);  // Retry every 5s if disconnected
}

// ============================================================
// MAIN LOOP
// ============================================================
void loop() {
  // 1. Process WebSocket frames (keep connection alive)
  webSocket.loop();

  // 2. Feed GPS data into TinyGPSPlus parser
  while (GPS_Serial.available() > 0) {
    gps.encode(GPS_Serial.read());
  }

  // 3. Send location every 3 seconds (only if GPS fix is valid)
  if (isConnected && (millis() - lastSendTime >= SEND_INTERVAL)) {
    lastSendTime = millis();

    if (gps.location.isValid()) {
      double lat = gps.location.lat();
      double lng = gps.location.lng();
      sendLocation(lat, lng);
    } else {
      // Print GPS debug info while waiting for fix
      Serial.print(" Waiting for GPS fix... ");
      Serial.print("Satellites: ");
      Serial.print(gps.satellites.isValid() ? gps.satellites.value() : 0);
      Serial.print("  HDOP: ");
      Serial.println(gps.hdop.isValid() ? gps.hdop.value() : 9999);
    }
  }
}