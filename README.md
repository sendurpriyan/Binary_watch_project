# Binary Type Wrist Watch

**Freelance Project – ESP32-Based Binary Watch with Neopixel LEDs**

---

## Project Overview
This project is a custom binary wristwatch powered by an **ESP32 microcontroller**. The watch displays time in binary format using **WS2812B Neopixel LEDs** and provides a **wireless web interface** for configuration. The project demonstrates embedded system design, PCB integration, and IoT connectivity.

---

## Features
- Binary time display using Neopixel LEDs
- Wireless configuration of:
  - LED colors
  - Brightness
  - Time format (12/24 hours)
- Compact and efficient hardware design
- ESP32-based embedded firmware
- Web server hosted on the ESP32 for easy client-side interaction

---

## Project Folder Structure

```text
Binary-Watch-Project/
│
├─ Firmware/        # Embedded C/C++ code for ESP32
│  ├─ binary_watch.ino
│  ├─ bw_u.ino
│  └─ final_draft.ino
│
├─ Hardware/        # Schematics and PCB files
│  ├─ flow_chart.docx
│  └─ flow_chart.pdf
│
├─ Docs/            # Images, diagrams, demo videos
│  └─ (add screenshots, photos, GIFs here)
│
├─ WebInterface/    # Web server or configuration files
│  └─ (HTML/CSS/JS or ESP32 web server scripts)
│
├─ README.md
└─ LICENSE

```
---

## Hardware Components
- ESP32-WROOM microcontroller
- WS2812B Neopixel LEDs
- Custom-designed PCB
- Power supply and connectors

---

## Software / Firmware
- Embedded C/C++ for ESP32
- Arduino IDE compatible
- Web server integration for BLE/Wi-Fi configuration

---

## Usage Instructions
1. Flash the firmware (`.ino` files) to the ESP32 using Arduino IDE.  
2. Connect the WS2812B LEDs to the ESP32 according to the schematic.  
3. Power up the device.  
4. Access the ESP32 web server via Wi-Fi to configure LED colors, brightness, and time format.  
5. The binary time will be displayed on the Neopixel LEDs in real time.  

---

## Demo / Media
*(Optional: Add images, GIFs, or videos here to showcase the working watch)*

---

## License
This project is open-source and licensed under the [MIT License](LICENSE).

---

**Author:** Sendurpriyan K  
**Role:** Freelance Embedded Systems Engineer
