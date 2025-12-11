#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <Adafruit_LSM6DSOX.h>

// ===== Disable Brownout (ESP32 WiFi power spikes) =====
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

// ======================
//  WiFi CONFIG
// ======================
const char* WIFI_SSID = "Sana_iPhone";
const char* WIFI_PASS = "Navid1234";

// ======================
//  AWS CONFIG
// ======================
// New API Gateway endpoint for /sensor on stage PROD
String AWS_URL = "https://fb9l1sywkl.execute-api.us-east-2.amazonaws.com/PROD/sensor";

// How often we are allowed to send a motion alert (ms)
const unsigned long SEND_INTERVAL_MS = 1500;

// ======================
//  SENSOR + ALERT CONFIG
// ======================
Adafruit_LSM6DSOX lsm;
const int LED_PIN    = 13;
const int BUZZER_PIN = 27;

// Adjust this if it is too sensitive or not sensitive enough
float MOTION_THRESHOLD = 1.0;   // m/s^2 difference

// Store the last accelerometer reading
sensors_event_t lastAccel;

// For rate limiting / anti spam
unsigned long lastSend = 0;
bool motionLock = false;        // prevents sending repeatedly while still moving

// For periodic WiFi status print
unsigned long lastWifiPrint = 0;

// =============== WiFi Helper ===============
void connectWiFi() {
  Serial.println();
  Serial.println("===== connectWiFi() CALLED =====");
  Serial.print("Target SSID: ");
  Serial.println(WIFI_SSID);

  // Slightly lower TX power to reduce brownout risk
  WiFi.mode(WIFI_STA);
  WiFi.setTxPower(WIFI_POWER_8_5dBm);

  Serial.println("Starting WiFi.begin()...");
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  int tries = 0;
  while (WiFi.status() != WL_CONNECTED && tries < 20) {
    delay(500);
    Serial.print(" WiFi status: ");
    Serial.print(WiFi.status());
    Serial.println(" (waiting...)");
    tries++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println(" WiFi connected inside connectWiFi()");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println(" WiFi FAILED inside connectWiFi(). Will retry later in loop.");
  }
}

// =============== AWS Send Helper ===============
void sendMotionAlertToAWS(float diff) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected, cannot send alert to AWS.");
    return;
  }

  // Build URL with query parameters
  String url = AWS_URL +
               "?diff="   + String(diff, 2) +
               "&motion=" + String(1);

  Serial.print("🌐 Sending alert to AWS: ");
  Serial.println(url);

  WiFiClientSecure client;
  client.setInsecure();   // for class/demo; skips certificate validation

  HTTPClient http;
  http.begin(client, url);

  int code = http.GET();
  if (code > 0) {
    Serial.print("AWS HTTP response code: ");
    Serial.println(code);
    String payload = http.getString();
    Serial.println("AWS response body:");
    Serial.println(payload);
  } else {
    Serial.print("HTTP error: ");
    Serial.println(code);
  }

  http.end();
}

// =============== SETUP ===============
void setup() {
  // Disable brownout detector
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("=== ESP32 Motion + LED + Buzzer + AWS Demo ===");
  Serial.println("Booting...");

  // LED + Buzzer pins
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);

  // I2C for ESP32: SDA = 21, SCL = 22
  Wire.begin(21, 22);

  // Initialize sensor
  Serial.println("Initializing LSM6DSOX...");
  if (!lsm.begin_I2C(0x6B)) {
    Serial.println("Failed to find LSM6DSO/LSM6DSOX at 0x6B");
    while (1) {
      delay(10);
    }
  }
  Serial.println("LSM6DSO found!");

  // Initialize lastAccel with the current reading
  sensors_event_t gyro, temp;
  lsm.getEvent(&lastAccel, &gyro, &temp);

  // Connect to WiFi once at startup
  connectWiFi();
}

// =============== LOOP ===============
void loop() {
  // Auto reconnect WiFi if needed
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected in loop(), calling connectWiFi() again...");
    connectWiFi();
  }

  // Periodic WiFi status (every 5 seconds)
  if (millis() - lastWifiPrint > 5000) {
    lastWifiPrint = millis();
    Serial.print("WiFi status check -> ");

    wl_status_t s = WiFi.status();
    if (s == WL_CONNECTED) {
      Serial.print("CONNECTED, IP = ");
      Serial.println(WiFi.localIP());
    } else {
      Serial.print("NOT CONNECTED, status code: ");
      Serial.println((int)s);
    }
  }

  // Read sensor
  sensors_event_t accel, gyro, temp;
  lsm.getEvent(&accel, &gyro, &temp);

  // Compute difference between current acceleration and last
  float dx = accel.acceleration.x - lastAccel.acceleration.x;
  float dy = accel.acceleration.y - lastAccel.acceleration.y;
  float dz = accel.acceleration.z - lastAccel.acceleration.z;

  float diff = sqrt(dx * dx + dy * dy + dz * dz);

  Serial.print("Motion diff: ");
  Serial.println(diff);

  bool motionDetected = (diff > MOTION_THRESHOLD);

  // ========= Local LED + Buzzer Alert =========
  if (motionDetected) {
    Serial.println("Motion DETECTED!");
    digitalWrite(LED_PIN, HIGH);
    digitalWrite(BUZZER_PIN, HIGH);
    delay(200);
    digitalWrite(LED_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);
  }

  // ========= Cloud Alert (AWS) with rate limiting =========
  unsigned long now = millis();

  if (motionDetected && !motionLock) {
    if (now - lastSend >= SEND_INTERVAL_MS) {
      lastSend = now;
      motionLock = true;   // lock until motion calms down
      sendMotionAlertToAWS(diff);
    }
  }

  // Unlock when motion calms down (so future movement can trigger again)
  if (!motionDetected && diff < 0.2) {
    motionLock = false;
  }

  // Update lastAccel for next loop
  lastAccel = accel;

  // Sampling delay (100 ms)
  delay(100);
}
