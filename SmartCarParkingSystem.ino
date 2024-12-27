#include <ESP8266WiFi.h>
#include <Servo.h>
#include <FirebaseESP8266.h>

// Replace with your network credentials
const char* ssid = "YourNetworkName";
const char* password = "YourNetworkPassword";

// Firebase configuration
#define FIREBASE_HOST "YourDatabaseURL"
#define FIREBASE_AUTH "YourAuthenticationToken"

// IR sensor and LED pin definitions
const int IR_Sensors[4] = {D1, D2, D3, D4};  // IR sensors for each garage space
const int entrySensorPin = D0;  // IR sensor at the entry gate to detect incoming vehicles
const int LED_Pins[2] = {D6, D7};  // LEDs to guide vehicles to available spaces
const int servoPin = D8;  // Servo motor to control the entry gate

// Servo and Firebase objects
Servo gateServo;
FirebaseData firebaseData;
FirebaseConfig config;
FirebaseAuth auth;

// Function to connect to WiFi with reconnection logic
void connectToWiFi() {
  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  // Keep retrying until WiFi is connected
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi!");
}

// Function to check and reconnect WiFi if connection is lost
void checkWiFiConnection() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi connection lost. Reconnecting...");
    delay(5000);
    connectToWiFi();
  }
}

// Function to update Firebase with garage space statuses
bool updateFirebase(const String& path, FirebaseJson& jsonData) {
  if (Firebase.setJSON(firebaseData, path, jsonData)) {
    return true;
  } else {
    Serial.print("Failed to update Firebase at ");
    Serial.print(path);
    Serial.print(": ");
    Serial.println(firebaseData.errorReason());
    return false;
  }
}

// Function to update the total number of empty spaces in Firebase
bool updateTotalEmptySpaces(int totalEmpty) {
  if (Firebase.setInt(firebaseData, "/totalemptyspace", totalEmpty)) {
    return true;
  } else {
    Serial.print("Failed to update total empty spaces: ");
    Serial.println(firebaseData.errorReason());
    return false;
  }
}

// Initial setup
void setup() {
  Serial.begin(115200);
  connectToWiFi();  // Establish WiFi connection

  // Configure Firebase with host and authentication details
  config.host = FIREBASE_HOST;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;
  Firebase.begin(&config, &auth);

  // Set up IR sensor and LED pin modes
  for (int i = 0; i < 4; i++) {
    pinMode(IR_Sensors[i], INPUT);
  }
  pinMode(entrySensorPin, INPUT);
  pinMode(LED_Pins[0], OUTPUT);
  pinMode(LED_Pins[1], OUTPUT);

  // Attach the servo and set the gate to the closed position
  gateServo.attach(servoPin);
  gateServo.write(45);  // Gate closed at 45 degrees
}

// State tracking variables
bool vehicleAtGate = false;  // Tracks if a vehicle is at the entry gate
bool vehicleParking = false;  // Tracks if a vehicle is currently being guided to park
int currentParkingSpace = -1;  // Tracks the available parking space being guided to

// Main loop
void loop() {
  checkWiFiConnection();  // Ensure WiFi connection is maintained

  bool garageStatus[4];  // Array to track the status of each garage space
  int availableSpace = -1;  // Variable to store the first available garage space
  int totalEmptySpaces = 0;  // Count of empty spaces

  // Check the status of each garage space
  for (int i = 0; i < 4; i++) {
    // If the sensor reads HIGH, the space is empty
    garageStatus[i] = (digitalRead(IR_Sensors[i]) == HIGH);
    if (garageStatus[i]) {
      totalEmptySpaces++;
      if (availableSpace == -1) {
        availableSpace = i;  // Store the first detected empty space
      }
    }
  }

  // Update Firebase if there is a change in the garage status
  static int lastTotalEmptySpaces = -1;
  static bool lastGarageStatus[4] = {false, false, false, false};

  if (totalEmptySpaces != lastTotalEmptySpaces || memcmp(garageStatus, lastGarageStatus, sizeof(garageStatus)) != 0) {
    FirebaseJson garageStatusJson;
    garageStatusJson.set("left", garageStatus[0]);
    garageStatusJson.set("center1", garageStatus[1]);
    garageStatusJson.set("center2", garageStatus[2]);
    garageStatusJson.set("right", garageStatus[3]);

    // Send updated status to Firebase
    if (updateFirebase("/garage", garageStatusJson)) {
      Serial.println("Updated garage space info in Firebase");
    }
    if (updateTotalEmptySpaces(totalEmptySpaces)) {
      Serial.print("Total empty spaces: ");
      Serial.println(totalEmptySpaces);
    }

    // Store the latest state for future comparisons
    lastTotalEmptySpaces = totalEmptySpaces;
    memcpy(lastGarageStatus, garageStatus, sizeof(garageStatus));
  }

  // Check if a vehicle is at the entry gate
  bool entrySensorState = digitalRead(entrySensorPin) == LOW;
  if (entrySensorState) {
    vehicleAtGate = true;
    gateServo.write(160);  // Open the gate
  } else {
    vehicleAtGate = false;
    gateServo.write(50);  // Close the gate
  }

  // Guide vehicle to available space if detected at the gate
  if (vehicleAtGate && availableSpace != -1) {
    vehicleParking = true;
    currentParkingSpace = availableSpace;
  }

  // Control guiding LEDs based on available space
  if (vehicleParking) {
    if (currentParkingSpace == 0) {
      digitalWrite(LED_Pins[0], LOW);
      digitalWrite(LED_Pins[1], LOW);
    } else if (currentParkingSpace == 1 || currentParkingSpace == 2) {
      digitalWrite(LED_Pins[0], !digitalRead(LED_Pins[0]));
      digitalWrite(LED_Pins[1], LOW);
    } else if (currentParkingSpace == 3) {
      digitalWrite(LED_Pins[0], !digitalRead(LED_Pins[0]));
      digitalWrite(LED_Pins[1], !digitalRead(LED_Pins[1]));
    }

    // Stop guiding if the vehicle parks in the designated space
    if (digitalRead(IR_Sensors[currentParkingSpace]) == LOW) {
      vehicleParking = false;
      currentParkingSpace = -1;
      digitalWrite(LED_Pins[0], LOW);
      digitalWrite(LED_Pins[1], LOW);
    }
  }
  delay(100);
}
