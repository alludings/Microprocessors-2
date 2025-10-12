// === Include Libraries ===
#include <Keypad.h>

// === Pin Definitions ===
// LEDs
const int redPin = 2;
const int yellowPin = 3;
const int greenPin = 4;

// Buzzer
const int buzzerPin = 5;

// === Keypad Setup ===
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {6, 7, 8, 9};
byte colPins[COLS] = {10, 11, 12, 13};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// === Variables ===
int redDuration = 0;
int greenDuration = 0;
const int yellowDuration = 3;

bool isStarted = false;

// === Helper Functions ===
void beepBuzzer(int duration) {
  tone(buzzerPin, 1000);
  delay(duration * 1000);
  noTone(buzzerPin);
}

void flashLED(int pin, int times, int interval_ms) {
  for (int i = 0; i < times; i++) {
    digitalWrite(pin, HIGH);
    delay(interval_ms);
    digitalWrite(pin, LOW);
    delay(interval_ms);
  }
}

int getDurationFromKeypad(char expectedKey) {
  String input = "";
  while (true) {
    char key = keypad.getKey();
    if (key) {
      if (key == '#') break;
      if (isDigit(key)) {
        input += key;
      }
    }
  }
  return input.toInt();
}

// === Setup Function ===
void setup() {
  pinMode(redPin, OUTPUT);
  pinMode(yellowPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  Serial.begin(9600);  // Optional: for debugging

  while (!isStarted) {
    digitalWrite(redPin, HIGH);
    delay(1000);
    digitalWrite(redPin, LOW);
    delay(1000);

    char key = keypad.getKey();
    if (key == 'A') {
      Serial.println("Enter Red Duration:");
      redDuration = getDurationFromKeypad('A');
    } else if (key == 'B') {
      Serial.println("Enter Green Duration:");
      greenDuration = getDurationFromKeypad('B');
    } else if (key == '*') {
      if (redDuration > 0 && greenDuration > 0) {
        isStarted = true;
        Serial.println("Starting Traffic Light Sequence...");
      }
    }
  }
}

// === Main Loop ===
void loop() {
  // RED light phase
  if (redDuration > 3) {
    digitalWrite(redPin, HIGH);
    delay((redDuration - 3) * 1000);
    beepBuzzer(3);
    flashLED(redPin, 3, 500);
    digitalWrite(redPin, LOW);
  } else {
    beepBuzzer(3);
    flashLED(redPin, redDuration, 500);
  }

  // GREEN light phase
  digitalWrite(greenPin, HIGH);
  if (greenDuration > 3) {
    delay((greenDuration - 3) * 1000);
    beepBuzzer(3);
    flashLED(greenPin, 3, 500);
  } else {
    beepBuzzer(3);
    flashLED(greenPin, greenDuration, 500);
  }
  digitalWrite(greenPin, LOW);

  // YELLOW light phase
  digitalWrite(yellowPin, HIGH);
  delay(yellowDuration * 1000);
  digitalWrite(yellowPin, LOW);
}


