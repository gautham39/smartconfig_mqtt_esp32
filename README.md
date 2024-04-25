# smartconfig_mqtt_esp32
## Overview
This project demonstrates how to initialize Wi-Fi in station mode on an ESP32, set up UART communication, and utilize SmartConfig for network configuration. This project demonstrates how to use an ESP32 to connect to a Wi-Fi network, establish MQTT client communication, and control a GPIO pin based on MQTT messages. It features MQTT event handling, SmartConfig integration, and LED control through MQTT messages.


## Dependencies
- ESP-IDF framework for ESP32 development.
- NVS (Non-Volatile Storage) for Wi-Fi credentials and other persistent storage.
- UART, Wi-Fi, and SmartConfig libraries.
- MQTT client library for ESP32.
- FreeRTOS for multitasking

- Usage
This project demonstrates how to establish MQTT communication with an ESP32 and control an LED through MQTT messages. Here's how to use it:

Connect the ESP32 to your computer for programming and monitoring.
After flashing, the ESP32 will initialize Wi-Fi and start MQTT communication.
The MQTT client connects to a broker (e.g., mqtt://test.mosquitto.org:1883).
Use an MQTT client (like MQTT Explorer or Mosquitto) to publish messages to the topic esp32/led/control.
Send "on" or "off" to control the LED state on GPIO pin 2.
The ESP32 publishes the LED status to /esp/led_status.
To reset Wi-Fi configuration with SmartConfig, send "smartconfig" to esp32/led/control

Contributing
Contributions are welcome! To contribute, please follow these steps:

Fork the repository and create a new branch.
Make your changes and test them.
Submit a pull request with a clear explanation of your changes.
