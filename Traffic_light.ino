#include <Keypad.h>

// === Pin Definitions ===
const int redPin = 2;
const int yellowPin = 3;
const int greenPin = 4;
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

// === State Machine ===
enum TrafficState {
  STATE_RED,
  STATE_GREEN,
  STATE_YELLOW
};
TrafficState currentState = STATE_RED;

// === Variables ===
int redDuration = 0;       // in seconds
int greenDuration = 0;     // in seconds
const int yellowDuration = 3;  // fixed 3 seconds

bool isStarted = false;

unsigned long stateStartTime = 0;
unsigned long currentTime = 0;
int stateDuration = 0;

// === Helper Functions ===
void beepBuzzer(int durationSec) {
  tone(buzzerPin, 1000);
  delay(durationSec * 1000);
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

// === Setup ===
void setup() {
  pinMode(redPin, OUTPUT);
  pinMode(yellowPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  Serial.begin(9600);

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
        currentState = STATE_RED;
        stateDuration = redDuration * 1000;
        stateStartTime = millis();
      }
    }
  }
}

// === Loop ===
void loop() {
  currentTime = millis();

  switch (currentState) {
    case STATE_RED:
      digitalWrite(redPin, HIGH);
      digitalWrite(greenPin, LOW);
      digitalWrite(yellowPin, LOW);

      if (currentTime - stateStartTime >= stateDuration - 3000 && currentTime - stateStartTime < stateDuration) {
        beepBuzzer(1);
      }

      if (currentTime - stateStartTime >= stateDuration) {
        currentState = STATE_GREEN;
        stateDuration = greenDuration * 1000;
        stateStartTime = currentTime;
      }
      break;

    case STATE_GREEN:
      digitalWrite(redPin, LOW);
      digitalWrite(greenPin, HIGH);
      digitalWrite(yellowPin, LOW);

      if (currentTime - stateStartTime >= stateDuration - 3000 && currentTime - stateStartTime < stateDuration) {
        beepBuzzer(1);
      }

      if (currentTime - stateStartTime >= stateDuration) {
        currentState = STATE_YELLOW;
        stateDuration = yellowDuration * 1000;
        stateStartTime = currentTime;
      }
      break;

    case STATE_YELLOW:
      digitalWrite(redPin, LOW);
      digitalWrite(greenPin, LOW);
      digitalWrite(yellowPin, HIGH);

      if (currentTime - stateStartTime >= stateDuration) {
        currentState = STATE_RED;
        stateDuration = redDuration * 1000;
        stateStartTime = currentTime;
      }
      break;
  }
}

