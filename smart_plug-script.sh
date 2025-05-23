#!/bin/bash

# ESP32 IP address
ESP_IP="192.168.18.127"

# Get battery percentage
BATTERY_PERCENT=$(upower -i /org/freedesktop/UPower/devices/DisplayDevice | grep percentage | awk '{print $2}' | sed 's/%//')

# Send to ESP32
curl -s -X POST http://$ESP_IP/battery \
     -H "Content-Type: text/plain" \
     -d "$BATTERY_PERCENT"
