/*************** LIBRARIES AND DEFINITIONS ***************/
#include <MPU6500_WE.h>  // Library for MPU6500 accelerometer/gyroscope
#include <Wire.h>        // I2C communication library
#define MPU6500_ADDR 0x68  // I2C address of MPU6500

// Create MPU6500 object
MPU6500_WE myMPU6500 = MPU6500_WE(MPU6500_ADDR);

/*************** PIN DEFINITIONS ***************/
const int BUZZER_PIN = 12;  // Buzzer output pin
const int LED_1 = 8;        // LED indicator for string 1
const int LED_2 = 9;        // LED indicator for string 2

// Capacitive touch sensor pins (for finger position detection)
const int SENSOR_PIN1 = 7;
const int SENSOR_PIN2 = 6;
const int SENSOR_PIN3 = 5;
const int SENSOR_PIN4 = 4;

/*************** MUSIC NOTES DEFINITIONS ***************/
// Notes for string 1 (C major scale)
const int string1Notes[] = {262, 294, 330, 349, 392}; // C4, D4, E4, F4, G4
// Notes for string 2 (G major scale)
const int string2Notes[] = {392, 440, 494, 523, 587}; // G4, A4, B4, C5, D5

/*************** STATE VARIABLES ***************/
int selectedString = 0;   // 0 = none, 1 = string1, 2 = string2
int noteIndex = 0;        // Current note index (0-4)
int noteFrequency = 0;    // Current note frequency in Hz
bool isNotePlaying = false; // Flag for active note playback

/*************** TIMING VARIABLES ***************/
unsigned long lastBowTime = 0;            // Last time bow motion was detected
unsigned long lastStringSelectionTime = 0; // Last time a string was selected
const unsigned long bowTimeout = 100;     // Stop note if no bowing for this time (ms)
const unsigned long stringTimeout = 50;   // Stop playing if string not selected recently (ms)

/*************** SENSOR THRESHOLDS ***************/
const float bowGyroThreshold = 30.0;      // Minimum angular velocity (deg/s) for bow detection

/*************** SETUP FUNCTION ***************/
void setup() {
  Serial.begin(115200);  // Initialize serial communication
  Wire.begin();          // Initialize I2C communication

  // Initialize MPU6500
  if (!myMPU6500.init()) {
    Serial.println("MPU6500 not found.");
    while (1);  // Halt if sensor not found
  }

  // Calibrate MPU6500
  Serial.println("Calibrating MPU6500...");
  delay(1000);
  myMPU6500.autoOffsets(); // Auto-calibrate sensor offsets
  Serial.println("Calibration done.");

  // Configure MPU6500 settings
  myMPU6500.enableGyrDLPF();  // Enable gyro digital low-pass filter
  myMPU6500.setGyrDLPF(MPU6500_DLPF_6); // Set filter bandwidth
  myMPU6500.setSampleRateDivider(5);     // Set sample rate divider
  myMPU6500.setGyrRange(MPU6500_GYRO_RANGE_250); // Set gyro range to ±250°/s
  myMPU6500.setAccRange(MPU6500_ACC_RANGE_2G);   // Set accelerometer range to ±2g
  myMPU6500.enableAccDLPF(true);         // Enable accelerometer DLPF
  myMPU6500.setAccDLPF(MPU6500_DLPF_6);  // Set accelerometer filter bandwidth

  // Configure pin modes
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);
  // Configure touch sensor pins as inputs
  pinMode(SENSOR_PIN1, INPUT);
  pinMode(SENSOR_PIN2, INPUT);
  pinMode(SENSOR_PIN3, INPUT);
  pinMode(SENSOR_PIN4, INPUT);
}

/*************** MAIN LOOP ***************/
void loop() {
  checkTiltAngle();     // Select string based on tilt (x-angle)
  checkGyroscope();     // Detect bowing using gyro Z-axis
  updateNoteState();    // Handle note playback
  delay(20);           // Small delay to stabilize loop timing
}

/*************** STRING SELECTION FUNCTION ***************/
void checkTiltAngle() {
  // Get current orientation angles from accelerometer
  xyzFloat angle = myMPU6500.getAngles();
  float xAngle = angle.x;  // Tilt angle around X-axis

  // Select string based on tilt direction
  if (xAngle <= -15) {
    selectedString = 1;          // String 1 selected (tilt left)
    digitalWrite(LED_1, HIGH);   // Turn on string 1 LED
    digitalWrite(LED_2, LOW);
  } 
  else if (xAngle >= 15) {
    selectedString = 2;          // String 2 selected (tilt right)
    digitalWrite(LED_1, LOW);
    digitalWrite(LED_2, HIGH);   // Turn on string 2 LED
  } 
  else {
    selectedString = 0;          // No string selected (neutral position)
    digitalWrite(LED_1, LOW);
    digitalWrite(LED_2, LOW);
  }

  // Update last selection time if a string is active
  if (selectedString != 0) {
    lastStringSelectionTime = millis();
  }
}

/*************** BOWING DETECTION FUNCTION ***************/
void checkGyroscope() {
  // Get current angular velocity from gyroscope
  xyzFloat gyr = myMPU6500.getGyrValues();
  float zRotation = abs(gyr.z);  // Absolute value of Z-axis rotation

  // Detect bowing motion if rotation exceeds threshold and string is selected
  if (selectedString != 0 && zRotation > bowGyroThreshold) {
    lastBowTime = millis();  // Update last bow detection time
  }
}

/*************** NOTE PLAYBACK FUNCTION ***************/
void updateNoteState() {
  unsigned long currentTime = millis();
  
  // Check if bowing is still active (within timeout period)
  bool bowingContinues = (currentTime - lastBowTime) <= bowTimeout;
  // Check if string selection is still active
  bool stringIsSelected = (currentTime - lastStringSelectionTime) <= stringTimeout;

  // Conditions for playing a note:
  // 1. Bowing motion detected recently
  // 2. String selection is recent
  // 3. A string is actually selected
  if (bowingContinues && stringIsSelected && selectedString != 0) {
    int newNoteIndex = getNoteFromSensor();  // Get current finger position
    // Determine frequency based on selected string and position
    int newFrequency = (selectedString == 1) ? string1Notes[newNoteIndex] : string2Notes[newNoteIndex];

    // Only change note if frequency changed or no note is playing
    if (newFrequency != noteFrequency || !isNotePlaying) {
      noteIndex = newNoteIndex;
      noteFrequency = newFrequency;
      tone(BUZZER_PIN, noteFrequency);  // Play the new note
      
      isNotePlaying = true;
    }
  } 
  else {
    // Stop playing if conditions aren't met
    if (isNotePlaying) {
      noTone(BUZZER_PIN);
      isNotePlaying = false;
      noteFrequency = 0;
    }
  }
}

/*************** FINGER POSITION DETECTION ***************/
int getNoteFromSensor() {
  // Check which touch sensor is active (priority to higher notes)
  if (digitalRead(SENSOR_PIN4) == HIGH) return 4;  // Highest position
  else if (digitalRead(SENSOR_PIN3) == HIGH) return 3;
  else if (digitalRead(SENSOR_PIN2) == HIGH) return 2;
  else if (digitalRead(SENSOR_PIN1) == HIGH) return 1;
  else return 0;  // Open string (no fingers)
}
