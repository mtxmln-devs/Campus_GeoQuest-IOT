# 🗺️ Campus GeoQuest - Real-Time IoT Geolocation Game

A real-time IoT geolocation game where teams connect Neo-M8N GPS modules to ESP32 microcontrollers and stream live location data to a central web server over secure WebSocket connections. Navigate the campus, earn points by reaching bot positions, and compete in an interactive GPS-based challenge!

![image alt]()


## 📖 Project Overview

**Campus GeoQuest** is a real-time IoT geolocation game that combines hardware, networking, and geospatial technology into an engaging competitive experience. Teams physically navigate campus grounds while their ESP32 devices stream GPS coordinates to a live web map, competing to reach designated bot positions and trigger collision-based scoring.

### 🎯 Learning Competencies

This project exercises the following technical competencies:

- **🔌 Hardware Integration**: Microcontroller wiring and UART serial communication
- **📡 Wireless Networking**: WiFi networking on embedded systems (ESP32 WiFi stack)
- **🔒 Secure Communication**: WebSocket (WSS) client communication over TLS/SSL
- **🛰️ GPS Processing**: NMEA sentence parsing and GPS coordinate extraction via TinyGPS++
- **📋 Data Serialization**: JSON serialization and structured message protocol design
- **⚡ Real-Time Architecture**: Real-time data streaming and event-driven IoT architecture

---

## 🛠️ System Features

### 1️⃣ Real-Time GPS Tracking & Processing
- **🔄 Asynchronous GPS Parsing**: Utilizes the `TinyGPSPlus` library to parse NMEA sentences without blocking the main execution loop
- **⚙️ Hardware UART Integration**: Dedicated hardware serial communication (UART2) for reliable data transfer from the GPS module
- **✅ Fix-Status Validation**: Intelligent monitoring of GPS lock, including satellite count and Horizontal Dilution of Precision (HDOP) metrics
- **📍 Coordinate Precision**: High-precision latitude/longitude tracking (6 decimal places)

### 2️⃣ Secure WebSocket Connectivity
- **🔐 WSS (Secure WebSocket) Support**: Implements SSL/TLS encryption (Port 443) for secure communication between ESP32 and remote server
- **🔄 Automated Heartbeat & Reconnection**: Built-in logic to handle network drops with 5-second auto-reconnect interval
- **📨 Event-Driven Architecture**: Uses callback handlers to manage connection states (Connect, Disconnect, Error, and Message Received)
- **🌐 Remote Server Integration**: Seamless connection to cloud-hosted WebSocket server

### 3️⃣ Efficient Data Serialization
- **📦 JSON Payload Management**: Uses `ArduinoJson` to package location data (Latitude, Longitude) and device metadata into lightweight, machine-readable strings
- **🎫 Dynamic Device Registration**: Automatically "announces" itself to the server upon connection, providing unique Device ID, name, and custom avatar URL
- **⏱️ Periodic Location Updates**: Sends location data every 3 seconds with optimized payload size

### 4️⃣ Robust Network Management
- **📶 WiFi Auto-Recovery**: Monitoring system that attempts to connect to mobile hotspot/router and triggers hardware restart if connection cannot be established
- **⏰ Non-Blocking Timing**: Employs `millis()` based scheduling for location updates (3-second intervals) instead of `delay()`, ensuring WebSocket connection remains active
- **🔁 Connection State Tracking**: Maintains connection status to prevent data transmission when disconnected

---

## 🎮 Game Mechanics

### 🏆 Scoring System
- **🎯 Automated Bot Detection**: System displays automated bots that roam campus at fixed interval paths
- **📏 Proximity-Based Collision**: Points awarded when GPS coordinates fall within proximity threshold (approximately 10-15 meters) of bot positions
- **⭐ Star Rewards**: Teams earn 3 stars when successfully navigating to bot positions
- **🏅 Competitive Leaderboard**: First team to earn 3 stars at the end of active period wins

### 🚶 Movement Requirements
- **📱 Physical Navigation**: Players must physically walk their ESP32 device along with mobile hotspot device to navigate
- **🔄 Update Frequency**: Positions update every 3 seconds on the live campus map
- **📊 Real-Time Tracking**: All team positions visible on interactive web interface

### ⚠️ Important Rules
> **REMINDER**: The GPS update interval is 3 seconds. Plan your movement accordingly — you need to be within proximity range for at least one update cycle for the collision to register.

---

## 🔧 Hardware Requirements

### 📦 Components per Group

| Component | Qty | Notes |
|-----------|-----|-------|
| **ESP32 Development Board** | 1 | Any 30-pin or 38-pin variant |
| **Neo-M8N GPS Module** | 1 | With ceramic patch antenna |
| **Micro-USB Cable** | 1 | For power and flashing |
| **Jumper Wires (Female-to-Female)** | 4 | TX, RX, VCC, GND |
| **Mobile Hotspot Device** | 1 | Phone with internet - acts as WiFi access point |
| **USB Power Bank** | 1 | To power your ESP32 device outdoors |

### 🔌 Wiring Diagram (Neo-M8N to ESP32)

| GPS Pin | ESP32 Pin | Description |
|---------|-----------|-------------|
| **VCC (3.3V)** | **3.3V** | Power supply — use 3.3V NOT 5V |
| **GND** | **GND** | Ground reference |
| **TX (UART out)** | **GPIO 16 (RX2)** | GPS transmit → ESP32 receive (UART2) |
| **RX (UART in)** | **GPIO 17 (TX2)** | GPS receive ← ESP32 transmit (UART2) |

> ⚠️ **WARNING**: Never connect the GPS VCC pin to the 5V rail of the ESP32. The Neo-M8N operates at 3.3V logic. Connecting to 5V will permanently damage the module.

---

## 💻 Software Setup

### 🔧 Arduino IDE Configuration

**Step 1: Install ESP32 Board Support**
1. Install ESP32 board support via **Boards Manager**: Search for "esp32" by Espressif Systems and install version 2.x or later

**Step 2: Install Required Libraries**
Open the Library Manager and install the following libraries:
- **WebSockets** by Markus Sattler (version 2.3.x or later)
- **TinyGPSPlus** by Mikal Hart (version 1.x)
- **ArduinoJson** by Benoit Blanchon (version 6.x)

**Step 3: Configure Board Settings**
- Set your board target to: **Tools > Board > esp32 > ESP32 Dev Module**
- Set Upload Speed to **115200** and Flash Frequency to **80MHz**

### 📚 Required Library Headers

Your sketch must include the following at the top:

```cpp
#include <WiFi.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>
#include <TinyGPSPlus.h>
#include <HardwareSerial.h>
```

---

## 🏗️ Code Architecture

### 4.1 Configuration Block

At the top of your code, declare your team's unique configuration constants. These values are transmitted to the server during device registration and appear on the live campus map.

```cpp
const char* ssid         = "YourHotspotName";      // Phone hotspot name
const char* password     = "YourPassword";         // Phone hotspot password
const char* ws_host      = "esp32-tracker.kiyokodyele.com"; // WebSocket server hostname
const uint16_t ws_port   = 443;                    // WSS port (SSL)
const char* device_id    = "ESP32_TeamName";       // "ESP32_" + team name (no spaces)
const char* device_name  = "TeamName";             // Team name
const char* avatar_url   = "https://...";          // Hosted image URL for your marker
```

> 📝 **NOTE**: Using a duplicate ID will overwrite another team's device on the map. Avatar URLs must be publicly accessible HTTPS links (PNG or JPG, max 128x128px recommended).

### 4.2 GPS and WebSocket Initialization

The `setup()` function must initialize four core subsystems in order: Serial monitor, GPS UART, WiFi connection, then WebSocket.

```cpp
void setup() {
  // 1. Serial Monitor
  Serial.begin(115200);
  
  // 2. GPS on UART2 (pins 16 RX, 17 TX)
  GPS_Serial.begin(9600, SERIAL_8N1, 16, 17);
  
  // 3. WiFi Connection
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  // 4. WebSocket with SSL
  webSocket.beginSSL(ws_host, ws_port, "/");
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(5000);
}
```

### 4.3 Main Loop Structure

The `loop()` function must perform **three tasks** every iteration:

1. **Call webSocket.loop()** — This processes incoming WebSocket frames and fires your event callback. Without this, the WebSocket connection will time out.

2. **Drain the GPS serial buffer** — Read all available bytes from `GPS_Serial` and pass each byte into `gps.encode()`. TinyGPSPlus parses NMEA sentences incrementally.

3. **Timed location sending** — Use `millis()` to track elapsed time and send a location update every 3 seconds only when a valid GPS fix is available.

```cpp
void loop() {
  webSocket.loop();
  
  while (GPS_Serial.available() > 0) {
    gps.encode(GPS_Serial.read());
  }
  
  if (isConnected && (millis() - lastSendTime >= SEND_INTERVAL)) {
    lastSendTime = millis();
    if (gps.location.isValid()) {
      sendLocation(gps.location.lat(), gps.location.lng());
    }
  }
}
```

### 4.4 WebSocket Event Handler

Your `webSocketEvent()` function must handle at minimum the following event types: `WStype_CONNECTED`, `WStype_DISCONNECTED`, `WStype_TEXT`, and `WStype_ERROR`. On `WStype_CONNECTED`, immediately call `registerDevice()` to announce your device to the server.

```cpp
void webSocketEvent(WStype_t type, uint8_t* payload, size_t length) {
  switch (type) {
    case WStype_CONNECTED:
      Serial.println("WebSocket Connected!");
      isConnected = true;
      registerDevice();
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
  }
}
```

### 4.5 Message Protocol (JSON)

The server expects two types of JSON messages sent as WebSocket text frames:

#### Registration Message — Sent Once on Connect

```json
{
  "type": "register",
  "deviceId": "ESP32_MALDITONG_AY",
  "name": "MALDITONG AY",
  "avatarUrl": "https://...",
  "latitude": 0.0,
  "longitude": 0.0
}
```

#### Location Update Message — Sent Every 3 Seconds

```json
{
  "type": "location",
  "latitude": 13.624356,
  "longitude": 123.185432,
  "avatarUrl": "https://..."
}
```

> ⚠️ **IMPORTANT**: Always check `gps.location.isValid()` before reading lat/lng values. Sending 0.0, 0.0 as a valid position will appear in the middle of the ocean on the map. Only transmit after a confirmed GPS fix.

---

## 🛰️ GPS Fix and Outdoor Operation

### 5.1 Understanding GPS Fix Time

**Cold Start GPS Acquisition**: A cold start (first power-on outdoors) can take **30 seconds to several minutes** to acquire a satellite fix. During this period, `gps.location.isValid()` returns `false`.

You can monitor progress in the Serial Monitor:

- **gps.satellites.value()** — Should increase as more satellites are acquired (need at least 4 for a position fix)
- **gps.hdop.value()** — Horizontal Dilution of Precision; lower is better (under 200 is acceptable)

### 5.2 Testing Sequence

1. **Flash your sketch** and open the Serial Monitor at **115200 baud**
2. **Take the assembled device outdoors** or near a window with clear sky view
3. **Wait for "WiFi connected!"** confirmation, followed by "WebSocket Connected!" confirmation
4. **Monitor the GPS output** — Wait for `"Location: VALID"` before beginning to walk and test
5. **Verify your team marker** appears on the live campus map shown by the instructor ([https://esp32-tracker.kiyokodyele.com/](https://esp32-tracker.kiyokodyele.com/)) ← Visit to confirm your code works

> 💡 **TIP**: GPS signal cannot penetrate concrete roofs or metal structures. Always test outdoors. The Neo-M8N must face upward (away from the ground). Indoors testing will not yield a satellite fix and your device will not move on the map.

---

## 🚨 Common Issues and Fixes

| Symptom | Likely Cause and Fix |
|---------|---------------------|
| **WiFi keeps reconnecting in a loop** | SSID or password is wrong. Mobile Hotspot is not broadcasting or is on 5GHz band only — switch to 2.4GHz. |
| **WebSocket never connects** | Incorrect ws_host or port. Confirm the server address with the instructor. Ensure SSL is enabled (beginSSL, not begin). |
| **"Waiting for GPS fix" never resolves** | Move outdoors and wait. GPS cold start can take up to 3 minutes. |
| **Serial Monitor shows garbled text** | Baud rate mismatch. Ensure Serial Monitor is set to 115200. |
| **Device shows on map at wrong location** | Stale GPS data from a previous session. Power cycle and wait for a fresh fix. |
| **JSON parse errors in server logs** | Malformed JSON from ArduinoJson — ensure `StaticJsonDocument` size (256 bytes) is sufficient and you call `serializeJson()` correctly. |

---

## 📊 System Architecture

### Data Flow Diagram

```
┌─────────────────┐
│   Neo-M8N GPS   │
│     Module      │
└────────┬────────┘
         │ NMEA sentences @ 9600 baud
         │ (UART2: GPIO 16 RX, 17 TX)
         ▼
┌─────────────────┐
│  ESP32 Device   │
│  ┌───────────┐  │
│  │ TinyGPS++ │  │ ◄── Parses NMEA
│  └───────────┘  │
│  ┌───────────┐  │
│  │ArduinoJson│  │ ◄── Serializes to JSON
│  └───────────┘  │
└────────┬────────┘
         │ WSS (Port 443) over WiFi
         │ JSON messages every 3s
         ▼
┌─────────────────┐
│  WebSocket      │
│     Server      │
│ (Cloud Hosted)  │
└────────┬────────┘
         │ Broadcasts to all clients
         ▼
┌─────────────────┐
│   Web Browser   │
│  (Live Campus   │
│      Map)       │
└─────────────────┘
```

---

## 📁 Project Structure

```
campus-geoquest-iot/
├── src/
│   └── midtProject-IOT.ino         # Main Arduino sketch
|
├── docs/
|   ├── hardwareGuide.docx      # Wiring and assembly instructions
│   ├── softwareGuide.docx      # Detailed code walkthrough
│   └── troubleshooting.docx    # Common issues and solutions
|
├── images/
│   ├── sample.jpg              # Sample Avatars
│   └── map.png                 # Game map screenshot
|
├── libraries/                  # Required Arduino libraries (reference)
│   ├── WebSockets/
│   ├── TinyGPSPlus/
│   └── ArduinoJson/
|
└── README.md                # This file
```

---

## 🎓 Educational Outcomes

### Technical Skills Developed

- **🔌 Embedded Systems**: Understanding microcontroller GPIO, serial protocols (UART), and power management
- **📡 Wireless Communication**: Implementing WiFi stack configuration, SSL/TLS handshaking, and WebSocket protocols
- **🛰️ Sensor Integration**: Working with GPS receivers, NMEA parsing, and coordinate systems
- **📊 Data Protocols**: JSON serialization, message framing, and event-driven communication patterns
- **🐛 Debugging**: Serial debugging, logic analyzer usage, and systematic troubleshooting

---

## 🤝 Contributing

We welcome contributions to improve the Campus GeoQuest system!

### How to Contribute

1. **🍴 Fork the Repository**
   ```bash
   git clone https://github.com/your-username/campus-geoquest-iot.git
   cd campus-geoquest-iot
   ```

2. **🌿 Create Feature Branch**
   ```bash
   git checkout -b feature/improved-gps-handling
   ```

3. **✨ Make Your Changes**
   - Improve GPS fix acquisition speed
   - Add battery monitoring features
   - Enhance error handling
   - Update documentation

4. **🧪 Test Thoroughly**
   - Test with actual hardware
   - Verify WebSocket stability
   - Check GPS accuracy

5. **📤 Submit Pull Request**
   - Provide clear description
   - Include test results
   - Reference related issues

---



**Key Points:**
- ✅ Free for educational and personal use
- ✅ Modification and distribution allowed
- ✅ Attribution required
- ✅ No warranty provided

---

## 🆘 Support & Resources

### 📚 Documentation
- **📖 Hardware Setup Guide** - Detailed assembly instructions
- **💻 Software Configuration** - Step-by-step code walkthrough
- **🔧 Troubleshooting Guide** - Solutions to common problems

### 💬 Getting Help
- **🐛 GitHub Issues**: Report bugs and technical problems
- **💡 Discussions**: Ask questions and share ideas
- **🌐 Live Map**: [https://esp32-tracker.kiyokodyele.com/](https://esp32-tracker.kiyokodyele.com/)

### 📖 Additional Resources
- **[ESP32 Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/)** - Official Espressif docs
- **[TinyGPS++ Library](http://arduiniana.org/libraries/tinygpsplus/)** - GPS parsing library
- **[WebSockets Protocol](https://datatracker.ietf.org/doc/html/rfc6455)** - RFC 6455 specification
- **[Neo-M8N Datasheet](https://www.u-blox.com/en/product/neo-m8-series)** - GPS module specifications

---

## 🎯 Game Tips & Strategy

### 🏃 Movement Strategy
- **📍 Plan Your Route**: Study bot positions before moving
- **⚡ Optimize Path**: Minimize distance between waypoints
- **🔄 Update Awareness**: Remember the 3-second update cycle
- **📱 Battery Management**: Bring fully charged power bank

### 🎮 Technical Tips
- **🛰️ GPS Warm-up**: Power on device 5 minutes before game start
- **📶 WiFi Strength**: Stay within hotspot range (10-15 meters)
- **🔋 Power Conservation**: Disable Bluetooth and unnecessary services
- **📊 Monitor Serial**: Keep Serial Monitor open for debugging

### ⚠️ Common Pitfalls
- **🌳 Tree Cover**: Dense foliage degrades GPS accuracy
- **🏢 Buildings**: Tall structures create GPS multipath errors
- **📡 Hotspot Range**: Device must stay within WiFi range
- **🔌 Connections**: Ensure jumper wires are firmly connected

---

## 🏆 Acknowledgments

### 👏 Special Thanks
- **Espressif Systems** - For the ESP32 platform
- **Mikal Hart** - TinyGPS++ library developer
- **Markus Sattler** - WebSockets library maintainer
- **Benoit Blanchon** - ArduinoJson creator
- **u-blox** - Neo-M8N GPS module manufacturer

---

<div align="center">

**Live Tracking Map**: [https://esp32-tracker.kiyokodyele.com/](https://esp32-tracker.kiyokodyele.com/)

Made with ❤️ for IoT Education | Follow the project for updates!

</div>

---

## 📊 Technical Specifications

### Hardware Specifications
- **Microcontroller**: ESP32 (Xtensa dual-core 32-bit LX6, 240 MHz)
- **WiFi**: 802.11 b/g/n (2.4 GHz)
- **GPS Module**: Neo-M8N (GPS/GLONASS, 72-channel)
- **GPS Accuracy**: 2.5m CEP (Circular Error Probable)
- **Update Rate**: 1Hz - 10Hz configurable
- **Power Consumption**: ~150mA @ 3.3V (active tracking)

### Software Specifications
- **Development Platform**: Arduino IDE 2.x
- **Board Package**: ESP32 by Espressif v2.x
- **Programming Language**: C/C++ (Arduino framework)
- **WebSocket Protocol**: RFC 6455 with WSS (TLS 1.2)
- **JSON Format**: Compact serialization, max 512 bytes
- **Update Interval**: 3000ms (configurable)

---

## 🔄 Version History

### v1.0.0 (Current Release)
- ✅ Initial release with core functionality
- ✅ GPS tracking and WebSocket communication
- ✅ Device registration and location updates
- ✅ Auto-reconnection logic
- ✅ JSON message protocol

---

