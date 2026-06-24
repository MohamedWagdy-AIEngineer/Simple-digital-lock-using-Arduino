#include <Keypad.h>
#include <LiquidCrystal.h>
#include <Servo.h>

#define SERVO_PIN 8
#define RED_LED_PIN 12
#define GREEN_LED_PIN 13
#define BUZZER_PIN 11

Servo myservo;
#define LOCK_POS 85
#define UNLOCK_POS 5
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);

const byte rows = 4, cols = 4;
char keys[rows][cols] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[rows] = {A0, A1, A2, A3};
byte colPins[cols] = {A4, A5, 9, 10};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, rows, cols);

char masterPassword[] = "*002007";
#define MAX_PASSWORD_LEN 10
#define MIN_PASSWORD_LEN 4

char savedPassword[MAX_PASSWORD_LEN + 1] = "";
char enteredPassword[MAX_PASSWORD_LEN + 1] = "";
int currentPosition = 0;
int invalidCount = 0;

bool isFirstBoot = true;
bool isSettingPassword = false;
bool isCheckingMaster = false;
bool isLockedOut = false;

void setup() {
  lcd.begin(16, 2);
  pinMode(RED_LED_PIN, OUTPUT);
  digitalWrite(RED_LED_PIN, LOW);
  pinMode(GREEN_LED_PIN, OUTPUT);
  digitalWrite(GREEN_LED_PIN, LOW);
  pinMode(BUZZER_PIN, OUTPUT);
  myservo.attach(SERVO_PIN);
  myservo.write(LOCK_POS);
  Serial.begin(9600);

  if (isFirstBoot) {
    lcd.print("Set Password:");
    isSettingPassword = true;
    currentPosition = 0;
  } else {
    lcd.print("*ENTER THE CODE*");
  }
}

void loop() {
  if (isLockedOut) return;

  char key = keypad.getKey();
  if (key) {
    playKeyPressSound();

    if (isSettingPassword) {
      handlePasswordSetup(key);
    } else if (isCheckingMaster) {
      handleMasterPasswordCheck(key);
    } else {
      handlePasswordEntry(key);
    }
  }
}

void playKeyPressSound() {
  tone(BUZZER_PIN, 1000, 100);
  delay(100);
}

void clearBuffer() {
  currentPosition = 0;
  enteredPassword[0] = '\0';
  memset(enteredPassword, 0, sizeof(enteredPassword));
}

bool isValidPasswordChar(char key) {
  return (key >= '0' && key <= '9') || key == '*' || key == '#';
}

void handlePasswordSetup(char key) {
  lcd.clear();

  if (key == 'B') {
    if (currentPosition > 0) {
      currentPosition--;
      enteredPassword[currentPosition] = '\0';
    }
  }
  else if (isValidPasswordChar(key)) {
    if (currentPosition < MAX_PASSWORD_LEN) {
      enteredPassword[currentPosition++] = key;
    }
  }
  else if (key == 'C') {
    if (currentPosition >= MIN_PASSWORD_LEN) {
      enteredPassword[currentPosition] = '\0';
      strncpy(savedPassword, enteredPassword, MAX_PASSWORD_LEN);
      savedPassword[MAX_PASSWORD_LEN] = '\0';
      clearBuffer();

      lcd.print("Password Set!");
      delay(2000);

      isSettingPassword = false;
      isFirstBoot = false;

      lcd.clear();
      lcd.print("*ENTER THE CODE*");
    } else {
      lcd.print("Incomplete Pass");
      delay(2000);
    }
    return;
  }

  lcd.setCursor(0, 0);
  lcd.print("Set New Pass:");
  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  for (int i = 0; i < currentPosition; i++) {
    lcd.print("*");
  }
}

void handleMasterPasswordCheck(char key) {
  lcd.clear();

  if (key == 'B') {
    if (currentPosition > 0) {
      currentPosition--;
      enteredPassword[currentPosition] = '\0';
    }
  }
  else if (isValidPasswordChar(key)) {
    if (currentPosition < MAX_PASSWORD_LEN) {
      enteredPassword[currentPosition++] = key;
    }
  }
  else if (key == 'C') {
    if (currentPosition >= MIN_PASSWORD_LEN) {
      enteredPassword[currentPosition] = '\0';

      if (strcmp(enteredPassword, masterPassword) == 0) {
        lcd.print("Master Verified");
        delay(2000);

        isCheckingMaster = false;
        isSettingPassword = true;
        clearBuffer();

        lcd.clear();
        lcd.print("Set Password:");
      } else {
        lcd.print("Incorrect Master");
        delay(2000);

        isCheckingMaster = false;
        clearBuffer();

        lcd.clear();
        lcd.print("*ENTER THE CODE*");
      }
    } else {
      lcd.print("Incomplete Pass");
      delay(2000);
    }
    return;
  }

  lcd.setCursor(0, 0);
  lcd.print("Master Pass:");
  lcd.setCursor(0, 1);
  for (int i = 0; i < currentPosition; i++) {
    lcd.print("*");
  }
}

void handlePasswordEntry(char key) {
  if (key == 'A') {
    lcd.clear();
    lcd.print("Master Pass:");
    clearBuffer();
    isCheckingMaster = true;
    return;
  }
  else if (key == 'B') {
    if (currentPosition > 0) {
      currentPosition--;
      enteredPassword[currentPosition] = '\0';
    }
  }
  else if (isValidPasswordChar(key)) {
    if (currentPosition < MAX_PASSWORD_LEN) {
      enteredPassword[currentPosition++] = key;
    }
  }
  else if (key == 'C') {
    if (currentPosition >= MIN_PASSWORD_LEN) {
      enteredPassword[currentPosition] = '\0';

      if (strcmp(enteredPassword, savedPassword) == 0) {
        invalidCount = 0;
        unlockDoor();
      } else {
        invalidCount++;
        incorrectPassword();

        if (invalidCount >= 3) {
          triggerLockout();
        }
      }
      clearBuffer();
    } else {
      lcd.clear();
      lcd.print("Incomplete Pass");
      delay(2000);
      lcd.clear();
      lcd.print("*ENTER THE CODE*");
    }
    return;
  }

  lcd.clear();
  lcd.print("PASSWORD:");
  lcd.setCursor(0, 1);
  for (int i = 0; i < currentPosition; i++) {
    lcd.print("*");
  }
}

void triggerLockout() {
  isLockedOut = true;

  for (int i = 10; i > 0; i--) {
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("SYSTEM LOCKED");
    lcd.setCursor(5, 1);
    lcd.print(i);
    lcd.print(" sec");

    digitalWrite(RED_LED_PIN, HIGH);
    tone(BUZZER_PIN, 800, 200);
    delay(200);
    digitalWrite(RED_LED_PIN, LOW);
    noTone(BUZZER_PIN);
    delay(800);
  }

  invalidCount = 0;
  isLockedOut = false;

  lcd.clear();
  lcd.print("*ENTER THE CODE*");
}

void unlockDoor() {
  lcd.clear();
  lcd.print("Access Granted");
  digitalWrite(GREEN_LED_PIN, HIGH);

  for (int i = 0; i < 4; i++) {
    tone(BUZZER_PIN, 400 + (i * 200), 200);
    delay(250);
  }
  noTone(BUZZER_PIN);
  digitalWrite(GREEN_LED_PIN, LOW);

  delay(2500);

  myservo.write(UNLOCK_POS);

  for (int sec = 10; sec > 0; sec--) {
    lcd.clear();
    lcd.print("Door locks in:");
    lcd.setCursor(6, 1);
    lcd.print(sec);
    lcd.print(" sec");
    delay(1000);
  }

  lcd.clear();
  lcd.print("Locking Door...");
  myservo.write(LOCK_POS);
  delay(1000);

  lcd.clear();
  lcd.print("*ENTER THE CODE*");
}

void incorrectPassword() {
  lcd.clear();
  lcd.print("CODE INCORRECT");
  digitalWrite(RED_LED_PIN, HIGH);

  for (int i = 0; i < 3; i++) {
    tone(BUZZER_PIN, 300, 150);
    delay(200);
    tone(BUZZER_PIN, 100, 150);
    delay(200);
  }
  noTone(BUZZER_PIN);
  digitalWrite(RED_LED_PIN, LOW);

  delay(2500);

  lcd.clear();
  lcd.print("*ENTER THE CODE*");
}
