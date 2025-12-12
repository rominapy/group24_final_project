# Smart Anti-Theft Motion Alert System

An IoT-based security device that detects suspicious motion of personal items (e.g., backpacks, handbags, laptop sleeves) and sends real-time alerts to the owner via cloud services.

---

## Table of Contents

1. [Overview](#overview)  
2. [Key Features](#key-features)  
3. [System Architecture](#system-architecture)  
4. [Hardware Components](#hardware-components)  
5. [Software Stack](#software-stack)  
6. [Getting Started](#getting-started)  
   - [1. Clone the Repository](#1-clone-the-repository)  
   - [2. Configure the Firmware](#2-configure-the-firmware)  
   - [3. Configure AWS IoT Core](#3-configure-aws-iot-core)  
   - [4. Flash the ESP32](#4-flash-the-esp32)  
7. [Usage](#usage)  
8. [Project Structure](#project-structure)  
9. [Future Improvements](#future-improvements)  
10. [Contributors](#contributors)  
11. [License](#license)

---

## Overview

The **Smart Anti-Theft Motion Alert System** is a portable IoT device designed to help protect personal belongings in public or semi-public spaces such as libraries, coffee shops, and shared offices.

The system combines:

- An **ESP32 TTGO** microcontroller with Wi-Fi,
- An **LSM6DSO accelerometer** for motion sensing,
- **AWS IoT Core**, **AWS Lambda**, **Amazon SNS**, and **CloudWatch** for cloud-based alerting and monitoring.

When abnormal motion is detected (e.g., someone picking up or moving the protected item), the device triggers local alarms (LED + buzzer) and publishes an alert message to the cloud, which then notifies the user.

---

## Key Features

- **Real-time motion detection** using a 3-axis accelerometer.
- **Local alerts** via LED and buzzer when suspicious movement is detected.
- **Secure Wi-Fi communication** using TLS and AWS IoT device certificates.
- **Cloud-based alerts** using MQTT → AWS IoT Core → Lambda → SNS (e.g., email/SMS).
- **Basic logging and monitoring** through Amazon CloudWatch (events and metrics).
- **Modular firmware**: separate tasks for sensing, alerting, and communication.

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
7. **Amazon SNS** sends notifications (email/SMS) to user  
8. **Amazon CloudWatch** logs events and supports simple dashboards


```markdown
