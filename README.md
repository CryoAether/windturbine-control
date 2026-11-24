# Wind Turbine Telemetry & Control System

Real-time embedded telemetry and remote control pipeline built with C++ (ESP32-S3, OpenGL/ImGui), MQTT, and Python to monitor and safeguard a wind turbine over a standalone local network.

## Why I Built This

I built this to monitor live electrical and environmental metrics—voltage, current, power, wind speed, temperature, humidity, and brake duty—from a wind turbine controller while keeping hardware protection responsive. Because relying on external routers adds latency and points of failure, the ESP32-S3 hosts a dedicated Wi-Fi SoftAP (`192.168.4.1`) paired with a local Mosquitto MQTT broker (`192.168.4.2:1883`) and automatic reconnection logic.

Using non-blocking `millis()` scheduling at 10 Hz (`100 ms`), the firmware reads a PZEM-017 sensor over RS485-to-TTL Modbus—starting with an initial blocking read in `setup()` so the PID controller initializes with true bus voltage rather than `0.0V`—and publishes 320-byte JSON payloads to `turbine/telemetry`. On the control path (`turbine/control/#`), an `ArduinoJson` callback handles real-time overvoltage threshold updates (`turbine/control/safeguard`) and software E-stops (`turbine/control/emergency`) that immediately clamp the dump-load PWM to 100%.

To visualize and validate the system, I built a live C++ monitoring client using OpenGL and ImGui alongside a synthetic Python telemetry simulator to stress-test JSON parsing and ingestion before the physical turbine hardware was ready. Early bench testing also uncovered ground-loop brownouts on an unisolated bus, leading to a hardware pivot from raw voltage dividers to galvanic and Hall-effect isolation.

## Key Features

- **Standalone Local Network:** ESP32-S3 configured as a dedicated Wi-Fi SoftAP (`192.168.4.1`) talking to a local Mosquitto MQTT broker (`192.168.4.2:1883`) with automatic reconnection.
- **Non-Blocking 10 Hz Telemetry:** `millis()`-scheduled loop aggregates 7 live metrics into a 320-byte JSON payload on `turbine/telemetry` without blocking `mqttClient.loop()`.
- **Event-Driven Remote Control:** Subscribes to `turbine/control/#` to dynamically adjust `dump_trip_voltage` or trigger an immediate software E-stop (`100%` dump-load PWM clamp).
- **Hardware-Safe Sensing:** Initializes PID state with a true bus voltage reading via PZEM-017 RS485 Modbus and uses galvanic/Hall-effect isolation to prevent ground-loop brownouts.
- **Live C++ Dashboard & Python Simulator:** OpenGL/ImGui desktop client for real-time telemetry visualization, paired with a Python simulator for pre-hardware pipeline stress testing.
