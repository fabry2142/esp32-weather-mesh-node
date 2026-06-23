# ESP32 Weather Mesh Node & Mini-Server

An open-source Edge IoT project engineered around the **ESP32 (NodeMCU-32S)** microcontroller, designed to function as a data-collection mini-server and a resilient mesh network node.

## 📊 System Architecture
1. **Edge Integration:** The ESP32 acts as a localized mini-server, executing HTTP requests to external Weather APIs.
2. **Data Parsing & Management:** The firmware processes incoming JSON payloads locally at the edge level.
3. **Telemetry Visualization:** Telemetry data is pushed into an analytical pipeline and visualized through customized **Grafana** dashboards for real-time environmental monitoring.
4. **Mesh Networking:** Node-to-node routing capabilities to ensure data persistence across local infrastructure.

## 🚀 Technologies Used
* **Hardware:** ESP32 (NodeMCU-32S) Microcontroller
* **Core Concepts:** Edge Computing, Mesh Networking, API Integration (JSON Parsing)
* **Monitoring:** Grafana Analytics
