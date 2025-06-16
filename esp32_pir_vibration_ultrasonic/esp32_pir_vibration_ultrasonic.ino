#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <DHT.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <HTTPClient.h>

// // mysql
unsigned long lastHttpSendTime = 0;
const unsigned long httpInterval = 10000;  // 10 seconds

// WiFi credentials
#define WIFI_SSID "Abby's"
#define WIFI_PASSWORD ""

// Firebase credentials
#define API_KEY ""
#define DATABASE_URL ""

// Firebase objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// Firebase helper files
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

// === Pin Configuration ===
// Ultrasonic
const int trigPin = 18;
const int echoPin = 5;
#define SOUND_SPEED 0.034  // in cm/us
// ultrasonic variable
long duration;
float distanceCm;
float distanceThreshold = 50.0;  // default fallback

// PIR Sensor
const int PIR_SENSOR_OUTPUT_PIN = 13;
// pir variable
volatile bool motionDetected = false;
unsigned long lastMotionTime = 0;
const unsigned long motionTimeout = 5000;  // 5 seconds
bool motionStatusShown = false;

// Vibration Sensor
const int vibrationPin = 25;
// vibration variable
bool vibrationDetected = false;
unsigned long lastVibrationTime = 0;
const unsigned long vibrationHoldTime = 5000;  // 5 second
unsigned long vibrationStartTime = 0;
const unsigned long vibrationCooldown = 5000;  // 5 seconds
bool vibrationDurationReset = false;

// DHT11
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
// dht variable
float temperature;
float humidity;

// Relay
const int relayPin = 17;
bool relayOn = false;
unsigned long relayTriggeredTime = 0;
const unsigned long relayHoldDuration = 5000;  // Relay stays on at least 5 seconds

// OLED Display
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  Serial.begin(115200);

  // OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED init failed");
    while (true)
      ;
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Starting system...");
  display.display();

  // Connect to WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");

  configTime(28800, 0, "pool.ntp.org", "time.nist.gov");  // 28800 = 8*3600 for Malaysia time

  // Firebase config
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  auth.user.email = "yungjielee@gmail.com";
  auth.user.password = "Asdfg12345";

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  Serial.println("Authenticating with Firebase...");
  while (auth.token.uid == "") {
    Serial.print(".");
    delay(500);
  }
  Serial.println("Firebase Ready!");

  // PIR setup
  pinMode(PIR_SENSOR_OUTPUT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIR_SENSOR_OUTPUT_PIN), pirISR, RISING);
  Serial.println("PIR Sensor warming up...");
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("PIR Warming Up...");
  display.display();
  delay(20000);  // Warm-up time for PIR
  Serial.println("PIR Ready!");
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("PIR Ready!");
  display.display();
  delay(1000);

  // Ultrasonic setup
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.println("Ultrasonic Ready!");

  // Vibration setup
  pinMode(vibrationPin, INPUT);
  Serial.println("Vibration Ready!");

  // DHT
  dht.begin();

  // Relay
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);

  // Final system ready message
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("System Ready!");
  display.display();
  delay(1500);
}

void loop() {
  unsigned long currentMillis = millis();

  // === Ultrasonic Measurement ===
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distanceCm = duration * SOUND_SPEED / 2;

  Serial.print("Distance (cm): ");
  Serial.println(distanceCm);


  // === PIR Motion Detection ===
  if (motionDetected) {
    Serial.println("Motion detected!");
    motionDetected = false;
    lastMotionTime = currentMillis;
    motionStatusShown = false;  // reset flag
  }

  if (!motionDetected && !motionStatusShown && (currentMillis - lastMotionTime > motionTimeout)) {
    Serial.println("No motion detected.");
    motionStatusShown = true;  // prevent further prints
  }

  // === Vibration Detection ===
  bool vibState = digitalRead(vibrationPin);
  if (vibState == HIGH) {
    if (!vibrationDetected) {
      vibrationDetected = true;
      vibrationStartTime = currentMillis;
      Serial.println("Detected vibration...");
    }
    lastVibrationTime = currentMillis;
  }
  if (vibrationDetected && currentMillis - lastVibrationTime > vibrationHoldTime) {
    Serial.println("No vibration detected.");
    vibrationDetected = false;
    vibrationDurationReset = false;  // Reset flag so cooldown can begin
  }

  // Track vibration duration
  int vibrationDuration = 0;
  if (vibrationDetected) {
    vibrationDuration = (currentMillis - vibrationStartTime) / 1000;  // seconds
    Serial.print("Vibration active for ");
    Serial.print(vibrationDuration);
    Serial.println(" seconds");
  }

  // === Vibration Duration Cooldown Reset ===
  static unsigned long vibrationCooldownStart = 0;
  if (!vibrationDetected && !vibrationDurationReset) {
    vibrationCooldownStart = currentMillis;
    vibrationDurationReset = true;
  }

  if (vibrationDurationReset && (currentMillis - vibrationCooldownStart >= vibrationCooldown)) {
    vibrationDuration = 0;
    Firebase.RTDB.setInt(&fbdo, "/wildRepellentSystem/vibrationDuration", vibrationDuration);
    Serial.println("Vibration duration reset after cooldown.");
    vibrationDurationReset = false;
  }


  // === DHT11 ===
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();
  Serial.print("Temp: ");
  Serial.print(temperature);
  Serial.print("°C, ");
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println("%");

  // === Relay ===

  // Manual turn on or off relay
  bool manualMode = false;
  bool manualRelay = false;

  if (Firebase.RTDB.getBool(&fbdo, "/wildRepellentSystem/manualMode")) {
    manualMode = fbdo.boolData();
  }

  if (Firebase.RTDB.getBool(&fbdo, "/wildRepellentSystem/relay")) {
    manualRelay = fbdo.boolData();
  }

  // Read distance threshold from Firebase
  if (Firebase.RTDB.getFloat(&fbdo, "/wildRepellentSystem/distanceThreshold")) {
    distanceThreshold = fbdo.floatData();

    // Optional: limit to prevent weird inputs
    if (distanceThreshold < 10 || distanceThreshold > 300) {
      distanceThreshold = 50.0;
    }
  } else {
    distanceThreshold = 50.0;  // fallback default
  }
  Serial.print("Distance Threshold: ");
  Serial.println(distanceThreshold);

  String statusMessage = "Safe.";
  bool triggerCondition = false;

  bool motionActive = (currentMillis - lastMotionTime < motionTimeout);
  bool distanceClose = (distanceCm <= distanceThreshold);

  // 1. Vibration only ≥ 3 seconds
  if (vibrationDetected && vibrationDuration >= 3 && !motionActive && !distanceClose) {
    statusMessage = "Danger: Vibration detected!";
    triggerCondition = true;
  }
  // 2. Motion only, distance > 50 cm
  else if (motionActive && !vibrationDetected && !distanceClose) {
    statusMessage = "Alert: Motion detected, but not near.";
    triggerCondition = false;
  }
  // 3. Motion + distance < 50 cm (no vibration)
  else if (motionActive && !vibrationDetected && distanceClose) {
    statusMessage = "Danger: Animal approaching!";
    triggerCondition = true;
  }
  // 4. Motion + vibration, distance > 50 cm
  else if (motionActive && vibrationDetected && vibrationDuration >= 3 && !distanceClose) {
    statusMessage = "Danger: Motion and vibration detected!";
    triggerCondition = true;
  }
  // 5. Motion + vibration + distance < 50 cm
  else if (motionActive && vibrationDetected && vibrationDuration >= 3 && distanceClose) {
    statusMessage = "Danger: Animal confirmed!";
    triggerCondition = true;
  }
  // 6. No motion, no vibration, distance > 50 cm
  else if (!motionActive && !vibrationDetected && !distanceClose) {
    statusMessage = "Safe: No activity.";
    triggerCondition = false;
  }
  // 7. Only distance < 50 cm, no motion/vibration
  else if (!motionActive && !vibrationDetected && distanceClose) {
    statusMessage = "Alert: Object close, no movement.";
    triggerCondition = false;
  }
  Serial.println(statusMessage);


  if (manualMode) {
    // Manual control
    digitalWrite(relayPin, manualRelay ? HIGH : LOW);
    relayOn = manualRelay;
    Serial.println(manualRelay ? "Relay ON (Manual)" : "Relay OFF (Manual)");

  } else {
    // Automatic trigger

    if (triggerCondition) {
      digitalWrite(relayPin, HIGH);
      relayOn = true;
      relayTriggeredTime = currentMillis;
      Serial.println("Relay turn on");

    } else if (relayOn && (currentMillis - relayTriggeredTime > relayHoldDuration)) {
      digitalWrite(relayPin, LOW);
      relayOn = false;
      Serial.println("Relay turn off");
    }
  }

  // === OLED Display ===
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Temp:");
  display.print(temperature, 1);
  display.print("C|");
  display.print("Hum:");
  display.print(humidity, 1);
  display.println("%");
  display.print("Dist: ");
  display.print(distanceCm, 1);
  display.println("cm");

  // Movement & Vibration & Status
  display.print("Move: ");
  if (currentMillis - lastMotionTime < motionTimeout) {
    display.print("Yes ");
  } else {
    display.print("No  ");
  }

  display.print("| Vib: ");
  display.println(vibrationDetected ? "Yes" : "No ");

  if (triggerCondition) {
    display.print("STATUS: ALERT ANIMAL!");
  } else {
    display.print("STATUS: Safe & Clear");
  }
  display.display();

  // Push sensor data to Firebase
  Firebase.RTDB.setFloat(&fbdo, "/wildRepellentSystem/temperature", temperature);
  Firebase.RTDB.setFloat(&fbdo, "/wildRepellentSystem/humidity", humidity);
  Firebase.RTDB.setFloat(&fbdo, "/wildRepellentSystem/distance", distanceCm);
  Firebase.RTDB.setBool(&fbdo, "/wildRepellentSystem/motion", (currentMillis - lastMotionTime < motionTimeout));
  Firebase.RTDB.setBool(&fbdo, "/wildRepellentSystem/vibration", vibrationDetected);
  Firebase.RTDB.setInt(&fbdo, "/wildRepellentSystem/vibrationDuration", vibrationDuration);
  if (!manualMode) {
    Firebase.RTDB.setBool(&fbdo, "/wildRepellentSystem/relay", relayOn);
  }
  Firebase.RTDB.setString(&fbdo, "/wildRepellentSystem/modeStatus", manualMode ? "Manual" : "Auto");
  Firebase.RTDB.setString(&fbdo, "/wildRepellentSystem/status", statusMessage);
  Firebase.RTDB.setFloat(&fbdo, "/wildRepellentSystem/distanceThreshold", distanceThreshold);

  String timestamp = getFormattedTime();
  Firebase.RTDB.setString(&fbdo, "/wildRepellentSystem/timestamp", timestamp);

  if (millis() - lastHttpSendTime >= httpInterval) {
    sendDataToMySQL(temperature, humidity, distanceCm, relayOn, vibrationDetected, motionActive, statusMessage, manualMode);
    lastHttpSendTime = millis();
  }

  delay(2000);
}

void pirISR() {
  motionDetected = true;
}

String getFormattedTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return "";
  }

  char timeStr[30];
  strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", &timeinfo);
  return String(timeStr);
}

void sendDataToMySQL(float temperature, float humidity, float distanceCm, bool relayOn, bool vibrationDetected, bool motionActive, String statusMessage, bool manualMode) {
  String serverName = "http://smartanimal.threelittlecar.com/insert_data.php";  // Replace with your real URL

  String httpRequest = serverName + "?temperature=" + String(temperature, 2)
                       + "&humidity=" + String(humidity, 2)
                       + "&distance=" + String(distanceCm, 2)
                       + "&relay=" + (relayOn ? "1" : "0")
                       + "&vibration=" + (vibrationDetected ? "1" : "0")
                       + "&motion=" + (motionActive ? "1" : "0")
                       + "&status=" + urlencode(statusMessage)
                       + "&mode=" + urlencode(manualMode ? "Manual" : "Auto");

  HTTPClient http;
  // Serial.println("HTTP GET: " + httpRequest);
  http.begin(httpRequest);
  int httpResponseCode = http.GET();
  if (httpResponseCode > 0) {
    Serial.print("HTTP Response (MySQL): ");
    Serial.println(httpResponseCode);
  } else {
    Serial.print("HTTP GET Error: ");
    Serial.println(httpResponseCode);
  }
  http.end();
}

String urlencode(String str) {
  String encoded = "";
  char c;
  char code0;
  char code1;
  for (int i = 0; i < str.length(); i++) {
    c = str.charAt(i);
    if (isalnum(c)) {
      encoded += c;
    } else {
      code1 = (c & 0xf) + '0';
      if ((c & 0xf) > 9) code1 = (c & 0xf) - 10 + 'A';
      code0 = ((c >> 4) & 0xf) + '0';
      if (((c >> 4) & 0xf) > 9) code0 = ((c >> 4) & 0xf) - 10 + 'A';
      encoded += '%';
      encoded += code0;
      encoded += code1;
    }
  }
  return encoded;
}
