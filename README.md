# Simple-digital-lock-using-Arduino
A secure, password-based digital lock system

# Simple Digital Lock System 🔒

A secure, standalone password-based digital lock system designed and implemented using Arduino. This project was developed as part of the **Electronics 1** course practical application at **Mansoura National University**.

The firmware has been engineered from scratch to move away from rigid, fixed-length locking mechanisms, introducing dynamic input behavior and a robust state-machine logic to handle security edge cases.

---

## 🛠️ Key Features

* **Dynamic Password Length:** Supports variable password lengths from **4 up to 10 digits**, significantly increasing security customization.
* **Input Expansion:** The validation logic accepts both numeric digits and special characters (`*` and `#`), widening the entropy of potential combinations.
* **Anti-Brute Force Lockout:** If an incorrect code is entered 3 consecutive times:
  * System inputs freeze completely.
  * An audio-visual alarm activates (Buzzer & Red LED).
  * A live **10-second countdown** is displayed on the LCD before allowing further attempts.
* **Administrative Master Override:** Embedded a dedicated administrative Master Password (`*002007`) to safely handle runtime system password resets and re-initialization.
* **Memory & Buffer Stabilization:** Optimized data handling using low-level memory clearing (`memset`) to prevent data corruption, runtime bugs, or residual garbage values from affecting the system state.
* **Automated Actuator Logic:** Drives a Servo motor to safe unlock positions, paired with an automated post-access LCD countdown timer before securely re-locking the door mechanism.

---

## ⚙️ Hardware Components

* **Microcontroller:** Arduino Uno (or compatible board)
* **Input:** 4x4 Matrix Keypad
* **Display:** 16x2 Character LCD
* **Actuator:** Servo Motor
* **Indicators:** Red LED, Green LED, and an active Buzzer.

---

## 📌 Pin Mapping

| Component | Arduino Pin | Description |
| :--- | :--- | :--- |
| **Servo Motor** | Pin 8 | PWM Control Pin |
| **Red LED** | Pin 12 | Status Indicator (Access Denied / Lockout) |
| **Green LED** | Pin 13 | Status Indicator (Access Granted) |
| **Buzzer** | Pin 11 | Audio Alerts & Alarm Sequences |
| **LCD (RS, E, D4-D7)**| Pins 2, 3, 4, 5, 6, 7 | Character Display Interface |
| **Keypad Rows** | Pins A0, A1, A2, A3 | Matrix Row Inputs |
| **Keypad Columns**| Pins A4, A5, 9, 10 | Matrix Column Inputs |

---

## 🎮 Keypad Functions

* **Key [ A ]:** Initiates the Master Password verification routine to change or reset the user password.
* **Key [ B ]:** Functions as a precise backspace/delete key during typing.
* **Key [ C ]:** Acts as the confirmation/enter key to submit passwords.

---

## 🚀 How It Works

1. **First Boot:** The system prompts the user to initialize a new secure password (4-10 digits) and saves it to the runtime memory.
2. **Normal Operation:** The system waits for code entry. 
   * **Correct Password:** Green LED lights up, access melody plays, and the Servo turns to `5°` (Unlocked) for a 10-second countdown before automatically returning to `85°` (Locked).
   * **Incorrect Password:** Red LED flashes, a warning sound plays, and `invalidCount` increments. Reaching 3 failures triggers the `SYSTEM LOCKED` state.

---

##  Planned Improvements

### Permanent Alarm Mode (Anti-Intrusion Enhancement)

A key security upgrade currently in development:

**Current Behavior:**
- After **3 consecutive wrong attempts**, the system triggers a 10-second
  audio-visual alarm countdown, then automatically resets and accepts input again.
- If the user fails **3 more times** after the reset, the cycle simply repeats.

**Planned Behavior:**
- First 3 wrong attempts → 10-second lockout countdown (unchanged).
- If the user fails **3 additional times** after the first lockout:
  * 🔴 Red LED and Buzzer activate **permanently** with no timer or auto-reset.
  * 🖥️ LCD displays a prompt to enter the **Master Password**.
  * 🔑 The **only way** to silence the alarm and restore the system
    is a correct Master Password entry.

**Security Rationale:** This two-tier lockout ensures that a brief alarm
is enough warning for a genuine mistake, but a persistent intruder triggers
a permanent alarm that requires physical administrative intervention to resolve.

