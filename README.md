# 🎻 Stringless ซอ: A Motion-Based Thai Saw U Instrument (Arduino Project)

> Created by Punyanutch Thongraung & Pasin Chadilalai  
> For CSS332 Microcontroller Mini Project @SIIT - Thammasat Univeristy Rangsit Campus


## 📖 Overview

The **Saw U** is a traditional Thai string instrument known for its warm, expressive sound. 
But like many string instruments, it comes with challenges: tuning is sensitive, and clean note production requires precision and practice.

This project is designed as a **proof-of-concept for a more accessible version of the instrument—especially for beginners**—by creating a **stringless, motion-controlled version** using gyroscopic sensors and tactile input, eliminating the need for manual tuning and reducing the precision needed to play clean notes.

---

## 🔧 Components Used

| Component                | Quantity | Description |
|--------------------------|----------|-------------|
| Arduino Uno/Nano         | 1        | Main controller board |
| MPU6500 Gyroscope/Accelerometer | 1  | Detects bowing (Z-axis) and tilt (X-axis) |
| Tactile Buttons (touch sensors) | 4 | Simulate finger position along string |
| Passive Buzzer           | 1        | Outputs the sound of selected note |
| LEDs                     | 2        | Indicate selected string |
| Breadboard & Jumper Wires| –        | For circuit prototyping |
| Resistors                | –        | Pull-down resistors for buttons (if needed) |

---

## 🧠 How It Works

### 🎻 Simulated Instrument Design
- **Bowing**: Detected through rotational movement (gyro Z-axis)
- **String Selection**: Determined by **tilting** the MPU6500 left or right (gyro X-axis)
- **Note Selection**: Four tacntile button simulate finger positions (open note to 4th finger)

### 🎵 Predefined Notes
- **String 1 (C major)**: C4, D4, E4, F4, G4 → 262, 294, 330, 349, 392 Hz  
- **String 2 (G major)**: G4, A4, B4, C5, D5 → 392, 440, 494, 523, 587 Hz
---

## 🧪 Features

- 🎵 2 virtual strings
- 👆 4 tactile button act as digital “fingering” for note selection
- 🎻 Motion-triggered bowing using gyroscope data
- 🔊 Basic sound output via buzzer
- 💡 String selection indicated via LEDs
- ⚙️ Auto-calibration on startup

---

## Limitations
- The buzzer produces only basic square wave tones—this is not meant to accurately mimic the timbre of a real Saw U.
- Finger detection is limited to binary on/off states for each note—no analog pressure or sliding detection.
- Intended as a demonstrative prototype, not a full musical instrument.

---

## 📦 Libraries Used

Make sure to install the following library via Arduino Library Manager:

- [`MPU6500_WE`](https://github.com/wollewald/MPU9250_WE) — used to interface with the MPU6500 gyroscope and accelerometer.

```cpp
#include <MPU6500_WE.h>
#include <Wire.h>

## 📸 Demo & Presentation

> View our full project presentation and live demo in the video:  
> 🔗 [Watch on YouTube](https://youtu.be/EHfukbABvKY?feature=shared)

## 📜 License

Open-source for educational and non-commercial use. Please give credit if you build upon it.

