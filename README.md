# 🦉 Smart Wild Animal Repellent System  

A **sensor-based IoT system** designed to detect and repel wild animals (like monkeys) from chicken coops or small farms. Built using **ESP32**, multiple sensors, and a **web-based monitoring dashboard**, the system provides real-time detection, alerts, and environmental monitoring.  

---

## 📌 Project Overview  

In rural and farm areas, wild animals such as monkeys often invade chicken coops, steal eggs, or harm livestock. Constant human supervision is not always possible, especially at night.  
The **Smart Wild Animal Repellent System** was developed to **detect**, **analyze**, and **repel** these animals automatically while allowing the owner to monitor the environment remotely.  

The system integrates:
- **Motion detection (PIR sensor)**
- **Distance measurement (Ultrasonic sensor)**
- **Vibration sensing**
- **Temperature and humidity tracking (DHT11)**
- **Relay-controlled alarm/speaker**
- **OLED display for real-time updates**
- **Web dashboard** for monitoring and control

---

## 🎯 Objectives  

- Detect the presence and distance of approaching animals.  
- Identify vibrations when the chicken coop is disturbed.  
- Automatically trigger an alarm/speaker when danger is confirmed.  
- Measure and display environmental conditions.  
- Provide remote monitoring and control via a web interface.  

---

## ⚙️ System Components  

| Component | Function |
|------------|-----------|
| **ESP32** | Main microcontroller unit |
| **PIR Sensor** | Detects motion near the area |
| **Ultrasonic Sensor** | Measures object distance |
| **Vibration Sensor** | Detects shaking or tampering |
| **DHT11** | Reads temperature and humidity |
| **Relay Module** | Activates alarm or speaker |
| **OLED Display** | Shows system status and sensor data |

---

## 🔌 Wiring Overview  

| Component | ESP32 Pin |
|------------|------------|
| DHT11 | GPIO4 |
| Relay Module | GPIO17 |
| OLED (SDA/SCL) | GPIO21 / GPIO22 |
| Ultrasonic (TRIG/ECHO) | GPIO18 / GPIO5 |
| Vibration Sensor | GPIO25 |
| PIR Sensor | GPIO13 |

**Power:** 3.3V or 5V depending on component  
**Ground:** Common GND for all sensors  

---

## 🌐 Web Interface  

Hosted at: [https://smartanimal.threelittlecar.com/](https://smartanimal.threelittlecar.com/)  

### Features:
- **Login & Register System**
- **Dashboard** showing live sensor readings  
- **Control Panel**
  - Auto mode → automatic detection  
  - Manual mode → user control  
- **Trend Page** (daily, weekly, monthly sensor data)
- **Adjustable Distance Threshold** for alarm trigger sensitivity  

---

## 🗄️ Database  

- **Firebase** – Real-time data retrieval  
- **PhpMyAdmin (MySQL)** – Historical data for trend visualization  

---

## 🧠 System Logic  

| Condition | Status Message | Alarm |
|------------|----------------|--------|
| Vibration only | Danger: Vibration detected! | ✅ Yes |
| Motion detected (not near) | Alert: Motion detected, but not near | ❌ No |
| Motion + close distance | Danger: Animal approaching! | ✅ Yes |
| Motion + vibration | Danger: Animal confirmed! | ✅ Yes |
| No activity | Safe: No activity | ❌ No |

---

## 🗂️ Project Structure  

```bash
Smart_Wild_Animal_Repellent_System/
├── esp32_pir_vibration_ultrasonic/   # Arduino code for ESP32
├── web_app/                          # Frontend website
├── backend/                          # PHP backend + database
└── README.md                         # Project overview
```

---

## 📽️ Demonstration Video  

🎥 [Watch on YouTube](https://youtu.be/lMrC7ET6_4g)

---

## 👨‍💻 Developed by  

**Lee Yung Jie**  
Universiti Utara Malaysia (UUM)  
Course: STTHK3113 – Sensor-Based Systems  
Semester: 2024/2025 (A242)  
Supervisor: Ahmad Hanis bin Mohd Shabli  
