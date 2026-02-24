# 💡 Smart Street Lighting System – C++ / Arduino

## 📌 Project Overview

This project presents an **Intelligent Public Lighting System** developed in C++ using Arduino.  
The system automatically adapts lighting based on human presence and operating modes.

🎯 Objective:  
Reduce energy consumption while improving safety and efficiency in urban lighting.

---

## 👨‍💻 Developed By

- Wael Guesmi
- Maha Romdhani

---

## 🧠 Concept

Traditional public lighting systems:
- Consume high energy
- Increase operational costs
- Impact the environment
- Have limited lifespan

Our intelligent system solves these issues by:
- Detecting presence using IR sensors
- Activating only necessary lamps
- Automatically turning off lights when the area is empty
- Managing multiple operating modes

---

## ⚙️ Technologies Used

### 🔹 Hardware
- Arduino Uno / Nano
- 3 LED lamps (PWM controlled)
- 4 IR motion sensors
- Push button (mode selection)
- Resistors
- Jumper wires

### 🔹 Software
- Arduino IDE
- C++
- Serial Monitor (for debugging & system monitoring)

---

## 🏗️ System Architecture (OOP Design)

The project is fully designed using:

- ✅ Abstraction  
- ✅ Inheritance  
- ✅ Polymorphism  
- ✅ Enumeration (Modes)

### Main Classes:

- `Composant` (Abstract base class)
- `Capteur` (Abstract)
- `CapteurIR` (Concrete sensor)
- `Luminaire` (Abstract)
- `Lampadaire` (Concrete lamp)
- `ModeLuminosite`
- `MessageManager`
- `ReseauLuminaire` (System core)

---

## 🌙 Operating Modes

| Mode | Description |
|------|------------|
| JOUR | All lamps OFF |
| NUIT | Smart detection mode |
| MAINTENANCE | All lamps ON |
| DEMO | Sequential blinking mode |

---

## 🔄 Working Principle

1. IR sensor detects movement
2. Corresponding lamp turns ON
3. Previous lamp turns OFF (progressive lighting)
4. Auto shutdown after timeout
5. Mode button cycles between system modes

---

## 🚀 Features

- Debounce logic for sensors
- PWM smooth fade-in / fade-out
- Object-Oriented clean architecture
- Real-time monitoring via Serial
- Movement progression tracking

---

## 📊 Applications

- Smart cities
- Residential areas
- Highways
- Parks
- Rural zones

---

## 🔮 Future Improvements

- Add LDR for ambient light detection
- Integrate IoT (ESP32 + WiFi monitoring)
- Energy consumption analytics
- Solar power integration

---

## 📸 Demonstration



---

## 📚 References

- https://docs.arduino.cc
- https://smartgrid.ieee.org
- C++ Programming Concepts

---

⭐ If you like this project, feel free to fork and contribute!
