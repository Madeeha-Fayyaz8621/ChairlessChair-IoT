/*
 * ======================================================================
 *  ChairlessChair-IoT
 *  Embedded Firmware for an IoT-Enabled Wearable Assistive System
 * ======================================================================
 *  Author      : Madeeha Fayyaz
 *  Project     : ChairlessChair – An Ergonomic Solution for Posture Support
 *  Institution : Air University, Islamabad
 *  Year        : 2025
 * ======================================================================
 *  Description:
 *
 *  ChairlessChair-IoT is an embedded firmware solution developed for an
 *  ergonomic wearable assistive system designed to reduce lower-limb
 *  fatigue during prolonged standing and sitting activities.
 *
 *  The system combines real-time Force Sensitive Resistor (FSR) feedback,
 *  IoT-based monitoring, and intelligent actuator control to provide
 *  autonomous posture assistance while allowing manual user control
 *  through a wireless web interface.
 *
 *  Two operating modes are implemented:
 *
 *  MODE 1 — Autonomous Control
 *    • Continuously monitors four FSR sensors
 *    • Detects user posture and load distribution
 *    • Automatically extends or retracts dual linear actuators
 *    • Provides real-time posture support
 *
 *  MODE 2 — Intelligent (Manual) Control
 *    • Wireless control through built-in WiFi web interface
 *    • Standing State button fully extends actuators
 *    • Sitting State button fully retracts actuators
 *    • Integrated movement timing and safety protection
 *
 * ======================================================================
 *  Features:
 *
 *    • Embedded firmware developed using Arduino C++
 *    • Arduino Uno R4 WiFi hosted web interface
 *    • Autonomous FSR-based posture detection
 *    • Intelligent manual control mode
 *    • Dual linear actuator synchronization
 *    • Relay-based actuator power management
 *    • Battery voltage monitoring
 *    • Real-time actuator state reporting
 *    • Safety timeout and actuator stop protection
 *    • Internal limit switch support
 *
 * ======================================================================
 *  Hardware:
 *
 *    • Arduino Uno R4 WiFi
 *    • 2 × LA21 Linear Actuators (150 mm Stroke)
 *    • 4 × Force Sensitive Resistors (FSRs)
 *    • 2 × BTS7960 (IBT-2) Motor Drivers
 *    • Relay Module
 *    • Voltage Sensor Modules
 *    • 12V Li-ion Battery Pack
 *    • DC-DC Buck Converter
 *    • Custom PCB
 *
 * ======================================================================
 *  Communication:
 *
 *    • WiFi Access Point (AP Mode)
 *    • Embedded HTTP Server
 *    • Real-time Web Dashboard
 *
 * ======================================================================
 *  Firmware Modules:
 *
 *    • WiFi Server
 *    • HTTP Request Handler
 *    • Autonomous Control Logic
 *    • Intelligent Control Logic
 *    • Battery Monitoring
 *    • Motor Driver Control
 *    • Safety Timer
 *    • Actuator State Machine
 *
 * ======================================================================
 */

// 1. Library Includes
#include <WiFiS3.h>

// 2. WiFi Configuration
const char* ssid = "Chairless Chair ioT system";
const char* password = "12345678";
WiFiServer server(80);

// 3. Relay & Battery Configuration
const int relayPin = 7;
const int batteryVoltagePin = A0;
const float voltageDividerFactor = 5.0;
const float maxBatteryVoltage = 12.6;

// 4. Global Variables (System States)
bool relayState = false;
bool intelligentControl = false;     // False = Autonomous Mode / True = Manual Override
bool manualExtending = false;
bool manualRetracting = false;
bool actuatorBusy = false;

String actuatorState = "Stopped";

// 5. Actuator Control Pins
#define RPWM1 5
#define LPWM1 6
#define RPWM2 9
#define LPWM2 10

// 6. FSR Sensor Pins
#define extSensorPin1 A4
#define extSensorPin2 A5
#define retSensorPin1 A2
#define retSensorPin2 A3

// 7. Sensor Calibration & Thresholds
int extBaseline1 = 100;
int extBaseline2 = 100;
int retBaseline1 = 100;
int retBaseline2 = 100;

const int noForceDeadband = 20; //Noise Filtering
const int extHeavyThreshold = 800; // Force Threshold
const int retHeavyThreshold = 800; // Force Threshold

// 8. Motor Parameters
const byte MOTOR_SPEED = 255;

// ==========================
// Hardcoded Motion Timing
// (Used in Intelligent Control)
// ==========================

// LA21 Actuator
// Stroke = 150 mm
// Speed = 42 mm/s

// Safety timing stops actuator slightly before the internal
// limit switch is expected to engage.

// 9. Safety Timing Variables
unsigned long movementStartTime = 0;

const unsigned long EXTEND_TIME = 3400;   // milliseconds
const unsigned long RETRACT_TIME = 3300;  // milliseconds

// =======================================================
// FUNCTION PROTOTYPES
// =======================================================

void extendActuators();
void retractActuators();
void stopActuators();

void setup() {

  Serial.begin(9600);

  // ==========================
  // Motor Driver Pins
  // ==========================
  pinMode(RPWM1, OUTPUT);
  pinMode(LPWM1, OUTPUT);

  pinMode(RPWM2, OUTPUT);
  pinMode(LPWM2, OUTPUT);

  // Stop actuators initially
  stopActuators(); //OR
  // analogWrite(RPWM1, 0);
  // analogWrite(LPWM1, 0);

  // analogWrite(RPWM2, 0);
  // analogWrite(LPWM2, 0);
  

  // ==========================
  // Relay Setup
  // ==========================
  pinMode(relayPin, OUTPUT);

  // Relay OFF at startup
  digitalWrite(relayPin, HIGH);

  // ==========================
  // Sensor Pins
  // ==========================
  pinMode(extSensorPin1, INPUT);
  pinMode(extSensorPin2, INPUT);

  pinMode(retSensorPin1, INPUT);
  pinMode(retSensorPin2, INPUT);

  // ==========================
  // Initialize System States
  // ==========================
  relayState = false;

  intelligentControl = false;

  manualExtending = false;
  manualRetracting = false;

  actuatorBusy = false;

  actuatorState = "Stopped";

  // ==========================
  // Start WiFi Access Point
  // ==========================
  WiFi.beginAP(ssid, password);

  Serial.println();
  Serial.println("Creating WiFi Access Point...");

  while (WiFi.status() != WL_AP_LISTENING)
  {
    delay(100);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Access Point Created Successfully!");

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // ==========================
  // Start Web Server
  // ==========================
  server.begin();

  Serial.println("HTTP Server Started");
}

// =======================================================
// MOTOR HELPER FUNCTIONS
// =======================================================

// Extend both actuators
void extendActuators()
{
    analogWrite(RPWM1, MOTOR_SPEED);
    analogWrite(LPWM1, 0);

    analogWrite(RPWM2, MOTOR_SPEED);
    analogWrite(LPWM2, 0);
}

// Retract both actuators
void retractActuators()
{
    analogWrite(RPWM1, 0);
    analogWrite(LPWM1, MOTOR_SPEED);

    analogWrite(RPWM2, 0);
    analogWrite(LPWM2, MOTOR_SPEED);
}

// Stop both actuators
void stopActuators()
{
    analogWrite(RPWM1, 0);
    analogWrite(LPWM1, 0);

    analogWrite(RPWM2, 0);
    analogWrite(LPWM2, 0);
}

void loop() {
  
// HTTP Request Handling
  
WiFiClient client = server.available();

if (client)
{
  // ========================================
// Read HTTP Request
// ========================================
String request = "";

unsigned long timeout = millis() + 1000;

while (client.connected() && millis() < timeout)
{
    if (client.available())
    {
        char c = client.read();
        request += c;

        if (request.endsWith("\r\n\r\n"))
            break;
    }
}

// ========================================
// Relay Control
// ========================================

if (request.indexOf("GET /on") != -1)
{
    digitalWrite(relayPin, LOW);
    relayState = true;
}

else if (request.indexOf("GET /off") != -1)
{
    digitalWrite(relayPin, HIGH);

    relayState = false;

    actuatorBusy = false;
    manualExtending = false;
    manualRetracting = false;

    actuatorState = "Stopped";

    stopActuators(); //OR
  
    // analogWrite(RPWM1,0);
    // analogWrite(LPWM1,0);

    // analogWrite(RPWM2,0);
    // analogWrite(LPWM2,0);
}

// ========================================
// Intelligent Control Toggle
// ========================================

else if(request.indexOf("GET /intelligent") != -1)
{

    intelligentControl = !intelligentControl;

    manualExtending = false;
    manualRetracting = false;

    actuatorBusy = false;

    actuatorState = "Stopped";
  
    stopActuators();
}

// ========================================
// Standing Button
// ========================================

else if(request.indexOf("GET /extend") != -1)
{

    if(intelligentControl && relayState)
    {

        manualExtending = true;
        manualRetracting = false;

        actuatorBusy = true;

        movementStartTime = millis();

    }

}

// ========================================
// Sitting Button
// ========================================

else if(request.indexOf("GET /retract") != -1)
{

    if(intelligentControl && relayState)
    {

        manualRetracting = true;
        manualExtending = false;

        actuatorBusy = true;

        movementStartTime = millis();

    }

}

// ========================================
// Real-Time Actuator State API
// ========================================

else if(request.indexOf("GET /state") != -1)
{

    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/plain");
    client.println("Connection: close");
    client.println();

    client.print(actuatorState);

    delay(1);

    client.stop();

    return;

}

// ========================================
// Read Arduino Battery Voltage
// ========================================

float batteryVoltage =
analogRead(batteryVoltagePin)
*
(5.0/1023.0)
*
voltageDividerFactor;

int batteryPercent =
constrain(
(batteryVoltage/maxBatteryVoltage)*100,
0,
100
);

// HTML Page
client.println("HTTP/1.1 200 OK");
client.println("Content-Type: text/html");
client.println();

client.println("<!DOCTYPE html>");
client.println("<html>");
client.println("<head>");

client.println("<meta name='viewport' content='width=device-width, initial-scale=1.0'>");
client.println("<title>Chairless Chair IoT System</title>");

client.println("<link href='https://fonts.googleapis.com/css2?family=Montserrat:wght@400;600&display=swap' rel='stylesheet'>");

client.println("<style>");

client.println("body{font-family:'Montserrat',sans-serif;background:#f4f4f4;margin:0;padding:0;text-align:center;}");

client.println("h1{background:#333;color:white;padding:18px;margin:0;}");

client.println(".section{padding:20px;}");

client.println(".flex{display:flex;justify-content:center;gap:40px;flex-wrap:wrap;}");

client.println(".card{background:white;padding:20px;border-radius:18px;box-shadow:0 0 10px rgba(0,0,0,.15);width:220px;}");

client.println(".circle{width:120px;height:120px;border-radius:50%;border:10px solid #17a2b8;margin:auto;position:relative;}");

client.println(".circleText{position:absolute;top:50%;left:50%;transform:translate(-50%,-50%);font-size:22px;font-weight:bold;}");

client.println(".button{padding:15px 35px;font-size:18px;border:none;border-radius:25px;color:white;cursor:pointer;}");

client.println(".green{background:#28a745;}");

client.println(".red{background:#dc3545;}");

client.println(".blue{background:#007bff;}");

client.println(".orange{background:#ff9800;}");

client.println(".state{font-size:22px;font-weight:bold;color:#007bff;}");

client.println("</style>");

client.println("</head>");

client.println("<body>");

client.println("<h1>Chairless Chair IoT System</h1>");


// =====================================================
// Relay Section
// =====================================================

client.println("<div class='section'>");

client.println("<h2>Relay Control</h2>");

client.print("<a href='/");
client.print(relayState ? "off" : "on");
client.print("'>");

client.print("<button class='button ");

client.print(relayState ? "red'>Turn OFF" : "green'>Turn ON");

client.println("</button>");

client.println("</a>");

client.println("</div>");


// =====================================================
// Battery Card
// =====================================================

client.println("<div class='section'>");

client.println("<div class='flex'>");

client.println("<div class='card'>");

client.println("<h3>Arduino Battery</h3>");

client.println("<div class='circle'>");

client.print("<div class='circleText'>");

client.print(batteryPercent);

client.println("%</div>");

client.println("</div>");

client.print("<h3>");

client.print(batteryVoltage,2);

client.println(" V</h3>");

client.println("</div>");

client.println("</div>");

client.println("</div>");


// =====================================================
// Actuator State
// =====================================================

client.println("<div class='section'>");

client.println("<div class='card' style='margin:auto;'>");

client.println("<h3>Actuator State</h3>");

client.println("<div id='actuatorState' class='state'>Loading...</div>");

client.println("</div>");

client.println("</div>");


// =====================================================
// Intelligent Control Toggle
// =====================================================

client.println("<div class='section'>");

client.print("<a href='/intelligent'>");

client.print("<button class='button blue'>");

client.print(intelligentControl ?
"Disable Intelligent Control"
:
"Enable Intelligent Control");

client.println("</button>");

client.println("</a>");

client.println("</div>");


// =====================================================
// Manual Override Buttons
// =====================================================

if(intelligentControl)
{

client.println("<div class='section'>");

client.println("<h2>Manual Override</h2>");

client.println("<div class='flex'>");

client.println("<a href='/extend'>");

client.println("<button class='button green'>Standing State</button>");

client.println("</a>");

client.println("<a href='/retract'>");

client.println("<button class='button orange'>Sitting State</button>");

client.println("</a>");

client.println("</div>");

client.println("</div>");

}


// =====================================================
// Footer
// =====================================================

client.println("<div class='section'>");

client.println("<p><b>Mode:</b> ");

client.println(intelligentControl ?
"Intelligent Control (Manual Override)"
:
"Autonomous Sensor Control");

client.println("</p>");

client.println("</div>");


// =====================================================
// JavaScript
// =====================================================

client.println("<script>");

client.println("setInterval(function(){");

client.println("fetch('/state')");

client.println(".then(response=>response.text())");

client.println(".then(data=>{");

client.println("document.getElementById('actuatorState').innerHTML=data;");

client.println("});");

client.println("},1000);");

client.println("</script>");

client.println("</body>");

client.println("</html>");

delay(1);
client.stop();
}

// ========================================
// Actuator Logic
// (Autonomous + Intelligent Control)
// ========================================

if (relayState)
{
    // ------------------------------------
    // Intelligent Control Mode
    // ------------------------------------
    if (intelligentControl)
    {
        if (manualExtending && actuatorBusy)
        {
            extendActuators();
            actuatorState = "Extending";
        }

        else if (manualRetracting && actuatorBusy)
        {
            retractActuators();
            actuatorState = "Retracting";
        }

        else
        {
            stopActuators();
            actuatorState = "Stopped";
        }
    }

    // ------------------------------------
    // Autonomous Mode
    // ------------------------------------
    else
    {
        int extAdj1 = analogRead(extSensorPin1) - extBaseline1;
        int extAdj2 = analogRead(extSensorPin2) - extBaseline2;

        int retAdj1 = analogRead(retSensorPin1) - retBaseline1;
        int retAdj2 = analogRead(retSensorPin2) - retBaseline2;

        // Deadband Filtering
        if (abs(extAdj1) < noForceDeadband) extAdj1 = 0;
        if (abs(extAdj2) < noForceDeadband) extAdj2 = 0;
        if (abs(retAdj1) < noForceDeadband) retAdj1 = 0;
        if (abs(retAdj2) < noForceDeadband) retAdj2 = 0;

        bool extending =
            (extAdj1 >= extHeavyThreshold) ||
            (extAdj2 >= extHeavyThreshold);

        bool retracting =
            (retAdj1 >= retHeavyThreshold) ||
            (retAdj2 >= retHeavyThreshold);

        // Serial Monitor
        Serial.print("Ext1: ");
        Serial.print(extAdj1);

        Serial.print(" | Ext2: ");
        Serial.print(extAdj2);

        Serial.print(" | Ret1: ");
        Serial.print(retAdj1);

        Serial.print(" | Ret2: ");
        Serial.println(retAdj2);

        if (extending)
        {
            extendActuators();
            actuatorState = "Extending";
        }

        else if (retracting)
        {
            retractActuators();
            actuatorState = "Retracting";
        }

        else
        {
            stopActuators();
            actuatorState = "Stopped";
        }
    }
}

// ------------------------------------
// Relay OFF
// ------------------------------------
else
{
    stopActuators();

    actuatorBusy = false;
    manualExtending = false;
    manualRetracting = false;

    actuatorState = "Stopped";
}

// =======================================================
// SAFETY TIMING LOGIC
// =======================================================

// Only active during Intelligent Control mode
if (relayState && intelligentControl && actuatorBusy)
{
    unsigned long elapsedTime = millis() - movementStartTime;

    // ---------- Standing State ----------
    if (manualExtending)
    {
        if (elapsedTime >= EXTEND_TIME)
        {
            stopActuators();

            manualExtending = false;
            actuatorBusy = false;

            actuatorState = "Stopped";

            Serial.println("Extension completed (Safety Stop)");
        }
    }

    // ---------- Sitting State ----------
    if (manualRetracting)
    {
        if (elapsedTime >= RETRACT_TIME)
        {
            stopActuators();

            manualRetracting = false;
            actuatorBusy = false;

            actuatorState = "Stopped";

            Serial.println("Retraction completed (Safety Stop)");
        }
    }
}
}
