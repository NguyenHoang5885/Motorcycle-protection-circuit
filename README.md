# 🚗 Vehicle Protection System

A vehicle protection system based on **ESP32**, **SIM A7680C**, **Neo-6M GPS**, and **MPU6050**, supporting **SMS alerts**, **GPS/LBS tracking**, and **Adafruit IO MQTT integration** for remote monitoring and theft detection.

---

## 🔧 Features

- 📍 **Dual-mode location tracking** using GPS (Neo-6M) and LBS (SIM A7680C)
- 🚨 **Theft detection** via MPU6050 accelerometer/gyroscope
- 📲 **Remote SMS commands** and alerts
- ☁️ **MQTT integration with Adafruit IO** for real-time cloud monitoring
- 🔗 **RESTful API-ready** for app/web dashboard integration
- 🗂️ **Data logging and history** for tracking events and locations

---

## 🧩 Hardware Components

| Component     | Function                              |
|---------------|---------------------------------------|
| ESP32         | Main controller with WiFi             |
| SIM A7680C    | GSM/GPRS module with LBS support      |
| Neo-6M GPS    | Satellite GPS module                  |
| MPU6050       | Motion sensor for theft detection     |
| Buzzer/LED    | Optional alert outputs                |

---

## 🌐 Communication Overview

- **SMS Commands** for control and status (e.g., request location)
- **MQTT Topics** for sending GPS, motion, and event data
- **REST API Ready** for future expansion


