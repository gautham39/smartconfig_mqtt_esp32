<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>SmartConfig MQTT ESP32</title>
</head>
<body>
    <h1>SmartConfig MQTT ESP32</h1>
    <h2>Overview</h2>
    <p>This project demonstrates how to initialize Wi-Fi in station mode on an ESP32, set up UART communication, and utilize SmartConfig for network configuration. It shows how to use an ESP32 to connect to a   Wi-Fi network, establish MQTT client communication, and control a GPIO pin based on MQTT messages. It features MQTT event handling, SmartConfig integration, and LED control through MQTT messages.</p>
    <h2>Dependencies</h2>
    <ul>
        <li>ESP-IDF framework for ESP32 development.</li>
        <li>NVS (Non-Volatile Storage) for Wi-Fi credentials and other persistent storage.</li>
        <li>UART, Wi-Fi, and SmartConfig libraries.</li>
        <li>MQTT client library for ESP32.</li>
        <li>FreeRTOS for multitasking.</li>
    </ul>
    <h2>Usage</h2>
    <p>This project demonstrates how to establish MQTT communication with an ESP32, control an LED through MQTT messages, and perform a FOTA update using a JSON command. Here's how to use it:</p>
    <ul>
        <li>Connect the ESP32 to your computer for programming and monitoring.</li>
        <li>After flashing, the ESP32 will initialize Wi-Fi and connect to Wi-Fi using the ESP-TOUCH APP by entering your Wi-Fi credentials and starting MQTT communication.</li>
        <li>The MQTT client connects to a broker (e.g., mqtt://test.mosquitto.org:1883).</li>
        <li>Use an MQTT client (like MQTT Explorer or Mosquitto) to publish messages to the topic <code>esp32/led/control</code>.</li>
        <li>Send "on" or "off" in JSON command  for example {"led":"on"} or {"led":"off"} to control the LED state on GPIO pin 2.</li>
        <li>The ESP32 publishes the LED status to <code>/esp/led_status</code>.</li>
        <li>To reset Wi-Fi configuration with SmartConfig, send {"smartconfig":"start"} to <code>esp32/led/control</code>.</li>
        <li>To update, send the JSON object as <code>{"update":"url"}</code>.</li>
    </ul>
    <h2>Contributing</h2>
    <p>Contributions are welcome! To contribute, please follow these steps:</p>
    <ol>
        <li>Fork the repository and create a new branch.</li>
        <li>Make your changes and test them.</li>
        <li>Submit a pull request with a clear explanation of your changes.</li>
    </ol>
</body>
</html>
