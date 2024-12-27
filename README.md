# IoT-based Smart Car Parking System using NodeMCU and Firebase Realtime Database
![IMG_20241227_182641](https://github.com/user-attachments/assets/d4f6c2f4-42c3-49ee-907f-dff8a6a5a74d)
### Brief
This project is a Smart Car Parking System that monitors parking space availability in real-time using ESP8266, IR sensors, and Firebase Realtime Database. The system updates a web dashboard to display the status of parking slots and can be scaled for larger parking areas.
### Target Use Case
Designed to optimize the parking management system in garages and public parking lots, the project ensures real-time monitoring of parking space availability. Future improvements aim to automate the booking process, allowing users to reserve parking spaces via a web app. This enables seamless parking in allocated or reserved spots without the need for manual supervision, enhancing efficiency and user experience.
### Future Improvements
- Add slot booking and mobile app integration.
- Enable payment systems and predictive analytics.
### Circuit Connections
| **Component**             | **Pin on ESP8266** | **Connection Description**                                                    |
|---------------------------|--------------------|-------------------------------------------------------------------------------|
| **IR Sensor-1 (Left)**    | D1                 | Signal pin to D1, VCC to 3.3V, GND to GND.                                    |
| **IR Sensor-2 (Center1)** | D2                 | Signal pin to D2, VCC to 3.3V, GND to GND.                                    |
| **IR Sensor-3 (Center2)** | D3                 | Signal pin to D3, VCC to 3.3V, GND to GND.                                    |
| **IR Sensor-4 (Right)**   | D4                 | Signal pin to D4, VCC to 3.3V, GND to GND.                                    |
| **Entry IR Sensor**       | D0                 | Signal pin to D0, VCC to 3.3V, GND to GND.                                    |
| **Servo Motor**           | D8                 | Control pin to D8, VCC and GND to an external 5V power source and GND.        |
| **LED-1 (Top-Left)**      | D6                 | Positive terminal to D6 via a 220Ω resistor, negative terminal to GND.        |
| **LED-2 (Top-Right)**     | D7                 | Positive terminal to D7 via a 220Ω resistor, negative terminal to GND.        |
| **ESP8266 Power**         | VIN, GND           | VIN to 5V power source, GND to ground.                                        |
