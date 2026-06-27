# Design Decisions

## ChairlessChair-IoT

### Engineering Design Decisions and Trade-Off Analysis

---

# Overview

The **ChairlessChair-IoT** project evolved through multiple hardware and firmware iterations to improve mechanical reliability, embedded system stability, user safety, and overall maintainability.

Rather than selecting components solely based on specifications, each engineering decision was guided by prototype evaluation, practical deployment constraints, integration complexity, and long-term reliability. The final system reflects a series of informed trade-offs aimed at delivering a robust wearable assistive device capable of autonomous posture support and wireless monitoring.

---

# 1. Microcontroller Selection

## Initial Design

**ESP32**

### Advantages

* Integrated WiFi and Bluetooth
* Higher processing capability
* Larger Flash and RAM
* Rich peripheral support

### Engineering Challenges

The ESP32 successfully powered the initial prototypes; however, as additional hardware, firmware features, and debugging requirements were introduced, maintaining the system became increasingly complex. The project prioritized deployment stability and simplified development during the final implementation phase.

---

## Final Design

**Arduino Uno R4 WiFi**

### Engineering Decision

The Arduino Uno R4 WiFi was selected for the final prototype because it provided:

* Integrated WiFi connectivity
* Simplified firmware development
* Stable GPIO performance
* Easier debugging
* Reliable long-term operation
* Improved maintainability near deployment

### Design Rationale

The transition from ESP32 to Arduino Uno R4 WiFi was made to simplify integration and improve overall system stability rather than due to processing or current limitations.

---

# 2. Pressure Sensor Selection

Multiple pressure sensing technologies were evaluated before selecting the final sensor for posture intention detection.

| Sensor                                    | Advantages                                                 | Limitations                                                                                                      |
| ----------------------------------------- | ---------------------------------------------------------- | ---------------------------------------------------------------------------------------------------------------- |
| **20 kg Load Cell**                       | Accurate force measurement                                 | Limited load capacity, required a full 3D-printed mounting sheet, rectangular shape restricted placement options |
| **50 kg Load Cell (Bathroom Scale Type)** | Higher load capacity                                       | Slow reset-to-zero behavior caused delays during posture transitions                                             |
| **Round FSR**                             | Simple installation                                        | Lower accuracy, slower response, delayed actuator activation                                                     |
| **Square FSR** *(Selected)*               | Compact, lightweight, fast response, practical integration | Lower absolute force accuracy compared to load cells                                                             |
| **HX711 + Load Cell Interface**           | High measurement resolution                                | Extremely sensitive, required extensive calibration and stable mounting                                          |

### General Observation

All evaluated sensors required some form of signal conditioning before reliable posture detection could be achieved.

---

## Final Design

**Square Force Sensitive Resistors (FSRs)**

### Engineering Decision

The square FSR sensors were selected because they offered:

* Simple mechanical integration
* Lightweight construction
* Compact footprint
* Fast response for posture transitions
* Minimal calibration effort
* Practical wearable implementation

### Design Rationale

Although FSRs provide lower force measurement accuracy than load cells, the project only required reliable intention detection rather than precise force measurement.

Combined with deadband filtering, baseline compensation, and threshold-based decision logic, the selected FSR sensors provided dependable performance while significantly reducing hardware complexity.

---

# 3. Motor Driver Selection

## Initial Design

**L298N Motor Driver**

### Advantages

* Low cost
* Widely available
* Suitable for rapid prototyping
* Easy PWM implementation

### Engineering Challenges

As the wearable system matured, several limitations became evident:

* Limited current handling capability
* Noticeable heat generation
* Voltage drop across the H-bridge
* Lower power efficiency
* Less suitable for continuous actuator operation

---

## Final Design

**BTS7960 (IBT-2) Motor Driver**

### Engineering Decision

The project transitioned to BTS7960 (IBT-2) motor drivers because they provide:

* High current capability
* Low power dissipation
* Reduced heating
* Reliable bidirectional control
* PWM speed control
* Better compatibility with LA21 linear actuators

### Design Rationale

The upgrade improved actuator reliability and thermal performance while providing a more robust platform for repeated sit-to-stand assistance.

---

# 4. Linear Actuator Selection

## Selected Actuator

* LA21 Electric Linear Actuator
* 12 V DC
* 150 mm Stroke

### Engineering Considerations

Several factors influenced actuator selection:

* Load capacity
* Stroke length
* Extension speed
* Physical dimensions
* Mounting compatibility
* Reliability
* Cost
* Availability

### Design Rationale

The LA21 actuator represented the most practical balance between mechanical performance and system integration for the wearable lower-limb support mechanism.

---

# 5. Power Architecture

## Engineering Decision

The final system uses **two electrically isolated power domains**.

### Battery 1

Supplies:

* Arduino Uno R4 WiFi
* FSR Sensors
* Control Electronics

### Battery 2

Supplies:

* Dual Linear Actuators
* BTS7960 Motor Drivers

### Design Rationale

Separating logic and actuator power minimizes voltage drops and electrical noise produced during actuator startup.

Benefits include:

* Stable controller operation
* Reduced electrical interference
* Improved firmware reliability
* Increased operational safety
* Better overall system robustness

---

# 6. Control Strategy

## Engineering Decision

A threshold-based intention detection algorithm was implemented instead of proportional force control.

### Sensor Processing Pipeline

* Baseline compensation
* Deadband filtering
* Threshold comparison
* Intention detection
* Motion command generation

### Benefits

* Lower computational overhead
* Improved robustness
* Reduced false triggering
* Faster decision making
* Reliable posture transition detection

---

# 7. Operating Modes

Two complementary operating modes were implemented.

## Autonomous Mode

The controller continuously:

* Reads FSR sensors
* Detects posture transitions
* Automatically extends or retracts actuators
* Provides real-time posture assistance

---

## Intelligent (Manual Override) Mode

Accessible through the WiFi dashboard.

Users can:

* Extend actuators (Standing State)
* Retract actuators (Sitting State)

### Engineering Purpose

* Maintenance
* Functional testing
* Demonstration
* Manual intervention
* Safe actuator verification

---

# 8. Safety Engineering

During prototype testing, one actuator exhibited unreliable behavior near its internal limit switch.

## Engineering Response

Software motion limits were introduced to stop actuator travel before the mechanical limit was reached.

Additional safety mechanisms include:

* Emergency actuator stop
* Relay isolation
* Motion timeout
* Internal limit switch support
* Manual override mode

### Benefits

* Reduced mechanical stress
* Prevention of actuator lock-up
* Improved operational reliability
* Enhanced user safety

---

# 9. IoT Architecture

## Engineering Decision

The Arduino Uno R4 WiFi hosts an embedded HTTP server operating in WiFi Access Point mode.

### Advantages

* No external router required
* Portable deployment
* Low communication latency
* Real-time local monitoring
* Simplified user connection

The dashboard supports:

* Relay control
* Battery monitoring
* Actuator state monitoring
* Manual override
* Operating mode switching

---

# 10. Firmware Architecture

The firmware was organized into modular functional components.

## Core Modules

* WiFi Server
* HTTP Request Handler
* Sensor Processing
* Autonomous Control Logic
* Manual Override Logic
* Battery Monitoring
* Motor Driver Control
* Safety Timer
* Actuator State Management

### Benefits

* Improved readability
* Easier debugging
* Better maintainability
* Simplified future expansion
* Cleaner firmware organization

---

# Future Engineering Improvements

Potential future enhancements include:

* FreeRTOS task scheduling
* Bluetooth Low Energy (BLE) interface
* MQTT communication
* AWS IoT integration
* OTA firmware updates
* Closed-loop actuator position control
* Current sensing and fault detection
* Brain–Computer Interface (BCI) integration using EEG and fNIRS signals

---

# Conclusion

The final **ChairlessChair-IoT** prototype represents an iterative engineering development process that balanced performance, reliability, manufacturability, and maintainability.

Instead of maximizing individual component specifications, the project emphasized practical system integration, robust embedded firmware, safe actuator control, and user-centered wearable design. The resulting platform demonstrates a multidisciplinary approach spanning embedded systems, firmware development, mechatronics, IoT, and human-assistive robotics while providing a strong foundation for future research and industrial development.
