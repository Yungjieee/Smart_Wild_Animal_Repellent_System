#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <DHT.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>

// WiFi credentials
#define WIFI_SSID "Abby's"
#define WIFI_PASSWORD ""

// Firebase credentials
#define API_KEY " "
#define DATABASE_URL " "

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

  // Connect to WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");

  // Firebase config
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  auth.user.email = "yungjielee@gmail.com";
  auth.user.password = " ";

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  Serial.println("Authenticating with Firebase...");
  while (auth.token.uid == "") {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nFirebase Ready!");


  // PIR setup
  pinMode(PIR_SENSOR_OUTPUT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIR_SENSOR_OUTPUT_PIN), pirISR, RISING);
  Serial.println("PIR Sensor warming up...");
  delay(20000);  // Warm-up time for PIR
  Serial.println("PIR Ready!");

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

  // OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED init failed");
    while (true)
      ;
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);


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
  bool triggerCondition = distanceCm < 50 || vibrationDetected || (currentMillis - lastMotionTime < motionTimeout);

  if (triggerCondition) {
    digitalWrite(relayPin, HIGH);
    relayOn = true;
    relayTriggeredTime = currentMillis;

  } else if (relayOn && (currentMillis - relayTriggeredTime > relayHoldDuration)) {
    digitalWrite(relayPin, LOW);
    relayOn = false;
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
  Firebase.RTDB.setBool(&fbdo, "/wildRepellentSystem/relay", relayOn);

  delay(2000);  // Short delay for fast loop
}

void pirISR() {
  motionDetected = true;
}
