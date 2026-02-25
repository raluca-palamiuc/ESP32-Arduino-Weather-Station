# ESP32-Arduino-Weather-Station
A dual-processor weather and air quality monitoring system built with Arduino Mega 2560 and ESP32-CAM. The system collects environmental data and provides real-time updates through a Telegram Bot and a Web Dashboard.



## Features:
- Real-time Monitoring: Tracks temperature, rain presence, and air quality (pollution levels).
- Arduino Mega 2560: Acts as the data acquisition layer, reading analog and digital signals from sensors.
- ESP32-CAM: Acts as the communication bridge, hosting a local Web Server for the dashboard and interfacing with the Telegram Bot API for remote alerts.
- Telegram Alerts: Sends immediate warnings for rain detection or high pollution and confirms when conditions return to normal.
- Interactive Commands: Use /status to get instant readings or /site to get the dashboard link via Telegram.
- Web Dashboard: Accessible via local IP to view current conditions in any browser.
