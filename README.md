# Smart Anti-Theft Motion Alert System

An IoT-based security device that detects suspicious motion of personal items (e.g., backpacks, handbags, laptop sleeves) and sends real-time alerts to the owner via cloud services.

---


## System Architecture

High-level data flow:

1. **Item with attached device**  
2. **LSM6DSO Accelerometer** measures motion and sends data via I²C.  
3. **ESP32 TTGO**  
   - Reads sensor data  
   - Runs the motion detection algorithm  
   - Controls LED and buzzer  
   - Publishes alerts using MQTT over Wi-Fi  
4. **Wi-Fi Router / Access Point**  
5. **AWS IoT Core (MQTT Broker)**  
6. **AWS Lambda** processes incoming MQTT messages  
7. **Amazon SNS** sends notifications (email) to user  
8. **Amazon CloudWatch** logs events and supports simple dashboards

# Smart Anti-Theft Motion Alert System

## Project Information

- **Course:** Internet of Things  
- **Project Title:** Smart Anti-Theft Motion Alert System  
- **Team Name:** Avengers  
- **Team #:** 24  
- **Members:**
  - Romina Pouya 
  - Sana Kimiagar 

---

## Project Overview

This project implements a small IoT device that can be attached to personal items (e.g., backpack, handbag, laptop sleeve) to detect theft-like motion and send alerts.

The system:

- Uses an **ESP32 TTGO** board and an **LSM6DSO accelerometer** to detect motion.
- Runs a simple threshold-based motion detection algorithm on the ESP32.
- Triggers a **local alarm** (LED + buzzer) when suspicious motion is detected.
- Sends a JSON alert message over **Wi-Fi** using **MQTT** to **AWS IoT Core**.
- Uses **AWS Lambda** and **Amazon SNS** to forward alerts to the user (e.g., email/SMS).
- Logs events to **CloudWatch** for basic monitoring.

The goal is to demonstrate an end-to-end IoT pipeline: sensing → edge processing → cloud messaging → user notification.

---

## Hardware Used

- ESP32 TTGO (Wi-Fi enabled)  
- LSM6DSO accelerometer (I²C)  
- Buzzer  
- LED  
- Push button  
- Breadboard + jumper wires  
- USB cable / power supply  

---

## System Architecture (Summary)

1. Item with attached device  
2. LSM6DSO accelerometer measures motion  
3. ESP32 TTGO:
   - Reads sensor data over I²C  
   - Detects abnormal motion  
   - Controls LED and buzzer  
   - Publishes MQTT alert to AWS IoT Core  
4. AWS IoT Core routes the message  
5. AWS Lambda processes the alert  
6. Amazon SNS sends a notification to the user  
7. CloudWatch logs events

---

## Repository Contents

At the top level this repository contains:

- **`FinalProject/`** – ESP32 firmware and project source code used on the device.  
- **`dash.html`** – HTML page for the simple CloudWatch / dashboard visualization.  
- **`esp32MotionDashboard-...`** – Exported AWS dashboard configuration files.  
- **`log-events-viewer-result (1–4).csv`** – Sample log files exported from CloudWatch for testing and analysis.  
- **`README.md`** – This file.

`.DS_Store` is an OS metadata file and can be ignored.

---






```markdown
Hardware Components
Component/Part	Quantity	Description
ESP32 TTGO (Wi-Fi enabled)	1	Main microcontroller with integrated Wi-Fi
LSM6DSO accelerometer	1	3-axis motion sensor (I²C interface)
Buzzer	1	Audio alert indicator
LED	1	Visual status / alert indicator
Push button	1	Device control (e.g., arming/disarming)
Breadboard	1	Prototyping board
Jumper wires	1 set	Connections between components
USB cable / power supply	1	Power and firmware flashing
