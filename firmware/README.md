# Firmware

## Overview

This directory contains the embedded firmware for the **ChairlessChair-IoT** project.

The firmware is developed for the **Arduino Uno R4 WiFi** and is responsible for sensor acquisition, actuator control, safety management, and the embedded web server used for wireless monitoring and manual operation.

---

## Main Firmware File

| File | Description |
|------|-------------|
| `ChairlessChair.ino` | Main firmware implementing autonomous posture detection, manual control, actuator management, battery monitoring, and IoT dashboard functionality. |

---

## Firmware Features

- Autonomous posture detection using four Force Sensitive Resistors (FSRs)
- Intelligent (Manual Override) control through a WiFi web dashboard
- Dual linear actuator synchronization
- Relay-controlled actuator power management
- Battery voltage monitoring
- Embedded HTTP server
- Real-time actuator state reporting
- Software safety timing for actuator protection
- Noise filtering and threshold-based intention detection

---

## Hardware Platform

- Arduino Uno R4 WiFi
- 2 × LA21 Linear Actuators
- 2 × BTS7960 (IBT-2) Motor Drivers
- 4 × Force Sensitive Resistors (FSRs)
- Relay Module
- Voltage Sensor Module
- Dual Battery Power Architecture

---

## Software Architecture

The firmware is organized into the following functional modules:

- WiFi Initialization
- Embedded HTTP Server
- Dashboard Request Handling
- Sensor Processing
- Autonomous Control Logic
- Manual Override Logic
- Actuator Driver Control
- Battery Monitoring
- Safety Timing
- Actuator State Management

---

## Development Environment

- Arduino IDE 2.x
- Arduino Uno R4 WiFi Board Package
- WiFiS3 Library

---

## Upload Instructions

1. Open `ChairlessChair.ino` in Arduino IDE.
2. Select **Arduino Uno R4 WiFi** as the target board.
3. Select the correct COM port.
4. Compile the project.
5. Upload the firmware to the board.
6. Open the Serial Monitor (9600 baud) to verify successful initialization.
7. Connect to the device's WiFi Access Point to access the web dashboard.

---

## Notes

This firmware represents the final embedded implementation developed during the ChairlessChair project. It integrates autonomous posture assistance with wireless monitoring while incorporating software-based safety mechanisms for reliable actuator operation.
