#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <DHT.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>

// WiFi credentials
#define WIFI_SSID "Abby's"
#define WIFI_PASSWORD "030427020738"

// Firebase credentials
#define API_KEY "AIzaSyBkC4ati0WYRzVGfOFXhA86y-_BhMFBYuw"
#define DATABASE_URL "https://esp-firebase-demo-2276c-default-rtdb.asia-southeast1.firebasedatabase.app/"

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
const unsigned long relayHoldDuration = 3000;  // Relay stays on at least 5 seconds


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

  // WiFi Connection
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Connecting WiFi...");
  display.display();

  // Connect to WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("WiFi Connected!");
  display.display();
  delay(1000);

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

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Firebase Ready!");
  display.display();
  delay(1000);


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
      Serial.println("Detected vibration...");
      vibrationDetected = true;
    }
    lastVibrationTime = currentMillis;
  }
  if (vibrationDetected && currentMillis - lastVibrationTime > vibrationHoldTime) {
    Serial.println("No vibration detected.");
    vibrationDetected = false;
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

  bool triggerCondition = distanceCm < 50 || vibrationDetected || (currentMillis - lastMotionTime < motionTimeout);

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
    display.print("ALERT: Animal!");
  } else {
    display.print("STATUS: Safe");
  }
  display.display();

  // Push sensor data to Firebase
  Firebase.RTDB.setFloat(&fbdo, "/wildRepellentSystem/temperature", temperature);
  Firebase.RTDB.setFloat(&fbdo, "/wildRepellentSystem/humidity", humidity);
  Firebase.RTDB.setFloat(&fbdo, "/wildRepellentSystem/distance", distanceCm);
  Firebase.RTDB.setBool(&fbdo, "/wildRepellentSystem/motion", (currentMillis - lastMotionTime < motionTimeout));
  Firebase.RTDB.setBool(&fbdo, "/wildRepellentSystem/vibration", vibrationDetected);
  if (!manualMode) {
    Firebase.RTDB.setBool(&fbdo, "/wildRepellentSystem/relay", relayOn);
  }
  Firebase.RTDB.setString(&fbdo, "/wildRepellentSystem/modeStatus", manualMode ? "Manual" : "Auto");
  Firebase.RTDB.setString(&fbdo, "/wildRepellentSystem/status", triggerCondition ? "Alert! Animal is detected by the sensor!" : "Safe. No Animal is detected.");

  String timestamp = getFormattedTime();
  Firebase.RTDB.setString(&fbdo, "/wildRepellentSystem/timestamp", timestamp);

  delay(2000);  // Short delay for fast loop
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
