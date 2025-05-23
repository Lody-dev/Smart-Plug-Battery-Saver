# 🧠 Smart Plug Battery Saver — Design Document
📝 Project Summary

A smart plug that prevents overcharging by cutting off power when a device battery reaches 80%. It uses an ESP32 microcontroller to control a relay and make decisions based on battery level data sent from a Linux device.

## ⚙️ System Overview

✅ Architecture

```bash
[ Linux Device ] ─────► [ ESP32 Smart Plug (HTTP Server) ]
      │                                 │
   Periodically                        Accepts battery %,
   sends battery %                    decides to cut or allow power
```
- ESP32 runs an HTTP server on the local Wi-Fi network.
- A script on the Linux device monitors the battery and sends battery % via HTTP POST.
- ESP32 receives this data and controls the relay accordingly.

## 🔌 Hardware Components


```bash
| Component              | Description                                              |
| ---------------------- | -------------------------------------------------------- |
| ESP32 Dev Board        | Hosts the HTTP server and controls the relay             |
| Relay Module           | Switches AC power on/off                                 |
| Power Cable + Socket   | Connects to wall and supplies controlled power to device |
| Power Source for ESP32 | USB or regulated power adapter                           |

```
## 💻 Software Components
### 1. ESP32 Firmware

   Written in C++

   Features:
```
        Wi-Fi connection (Station mode)

        HTTP Server on ESP32 (e.g., port 80)

        Endpoint /battery to receive POST request: { "percent": 78 }
```
Relay control logic:

    If percent ≥ 80 → relay OFF (cut power)

    If percent < 80 → relay ON (allow charging)
### 2. Linux Script

 Written in Bash or Python

   Tasks:
```
        Read battery level (via upower, acpi, or /sys/class/power_supply)

        Send HTTP POST to ESP32’s /battery endpoint every ~30 seconds
```

## License

[MIT](https://choosealicense.com/licenses/mit/)
