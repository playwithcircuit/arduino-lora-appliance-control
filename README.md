# Arduino LoRa Long-Range Appliance Control System

<img src="images/Long Range Arduino-based Wireless Appliance Control using Mobile.jpg" width="800">

LoRa (Long Range) communication enables devices to exchange data over large distances while consuming very little power. This technology is widely used in IoT applications such as remote monitoring, smart agriculture, and industrial automation.

In this project, we build a long-range appliance control system using Arduino Nano and the RYLR999 LoRa + BLE module. A smartphone connects to the system using Bluetooth Low Energy (BLE) and sends control commands. These commands are then transmitted through LoRa wireless communication to a remote node, which activates relays to control electrical appliances such as a bulb or fan.

The project demonstrates how BLE and LoRa technologies can be combined to create a wireless control system that works even without Wi-Fi or internet connectivity.

---

## Project Overview

The system consists of two nodes:

**Controller**

The controller node receives commands from a smartphone using BLE communication. The Arduino processes the received commands and transmits them through the LoRa module.

**Target**

The target node receives the LoRa message and executes the command by switching the relay module connected to the appliances.
The LCD displays provide real-time information about system status, command transmission, and acknowledgement messages.

---
### Communication Flow

```
Smartphone
   ↓ BLE
Controller Arduino
   ↓ LoRa
Target Arduino
   ↓
Relay Module
   ↓
Appliance
```

## Components Required

- Arduino Nano (2 units)
- RYLR999 LoRa + BLE Module (2 units)
- 16×2 LCD Display with I2C Interface (2 units)
- 5V Bidirectional Voltage Level Shifter (2 units)
- 2-Channel Relay Module
- Jumper Wires
- 12V Power Adapter
- USB Cable for Arduino
---

## Key Features of the RYLR999 Module

The Reyax RYLR999 module integrates **LoRa communication and Bluetooth Low Energy in a single compact module**.

Important characteristics:

- Supports long-range wireless communication
- BLE interface allows smartphone connectivity
- Uses UART communication for easy integration with microcontrollers
- Configurable using AT commands
- Suitable for IoT and remote monitoring applications
---

## Working Principle

The system begins with a smartphone acting as the user interface. Using the LightBlue BLE mobile application, the smartphone connects to the RYLR999 module through Bluetooth Low Energy.

When the user presses a button in the mobile application, a control command is transmitted via BLE to the controller node. The Arduino Nano reads this command through the BLE interface of the module and interprets the instruction.

Once the command is identified, the Arduino forwards the command to the LoRa interface of the module. The RYLR999 module then transmits the data wirelessly using LoRa communication.

At the target node, another RYLR999 module receives the transmitted message and forwards it to the second Arduino Nano. The Arduino processes the received command and activates the corresponding relay channel.

The relay switches its internal contacts, allowing the connected appliance fan and bulb to turn ON or OFF. After executing the command, the target node sends a confirmation message back to the controller node.

Throughout the process, the LCDs show system messages such as module initialization, received commands, transmission status, and acknowledgement messages.

---

## Circuit Connections

### Controller Node Connections

| Arduino Nano Pin | RYLR999 Pin | Purpose |
|------------------|-------------|--------|
| 5V | VDD | Power supply to module |
| GND | GND | Common ground |
| RX (Pin 0) | TXD_LoRa (via voltage shifter) | Arduino receives LoRa data |
| TX (Pin 1) | RXD_LoRa (via voltage shifter) | Arduino sends LoRa commands |
| Pin 2 | RXD_BLE (via voltage shifter) | Arduino sends BLE data |
| Pin 3 | TXD_BLE (via voltage shifter) | Arduino receives BLE data |

### LCD Connections

| Arduino Nano | LCD Pin |
|--------------|---------|
| A4 | SDA |
| A5 | SCL |
| 5V | VCC |
| GND | GND |

---

### Target Node Connections

| Arduino Nano Pin | Device | Purpose |
|------------------|--------|--------|
| RX (Pin 0) | TXD_LoRa | Receive LoRa data |
| TX (Pin 1) | RXD_LoRa | Send LoRa response |
| D11 | Relay Channel 1 | Bulb control |
| D12 | Relay Channel 2 | Fan control |
| 5V | Relay VCC | Power supply |
| GND | Relay GND | Ground |

---

## Command Structure

### Commands from Smartphone (BLE)

```
*L1# → Turn bulb ON
*L0# → Turn bulb OFF
*F1# → Turn fan ON
*F0# → Turn fan OFF
```

### Commands Received at Target (LoRa)

```
L1 → DONE
L0 → DONE
F1 → DONE
F0 → DONE
```

---

## Range Observation

During practical testing performed in a dense urban environment with multiple buildings and obstacles, the following communication ranges were observed:

- Approximately **500 meters in line-of-sight conditions**
- Approximately **250 meters in non-line-of-sight conditions**

The achievable range of LoRa communication depends on several factors including antenna quality, transmission parameters, environmental conditions, and interference. Under open environments with optimized antennas, LoRa communication can typically reach several kilometers.

---

## Applications

This system architecture can be used in many practical applications such as:

- Remote equipment control
- Smart agriculture systems
- Industrial monitoring
- Wireless lighting systems
- Off-grid automation
- IoT device control networks

---

## Learning Outcomes

Through this project, you will learn:

- How LoRa communication works
- Interfacing the RYLR999 module with Arduino
- Using BLE communication for smartphone control
- Designing long-range wireless systems
- Controlling high-voltage appliances safely using relays

---

## Troubleshooting

If the system does not work correctly, check the following:

- Ensure correct UART connections between Arduino and RYLR999 module.
- Verify the voltage level shifter connections.
- Confirm that the LoRa module addresses are configured correctly.
- Check antenna connections and placement.
- Ensure the BLE mobile application is sending the correct command format.

---

## Future Enhancements

This project can be extended further by adding additional features such as:

- Monitoring sensor data over LoRa
- Creating a multi-node LoRa network
- Adding mobile application interface for appliance status
- Integrating with cloud platforms
- Implementing encryption for secure communication

---
## Repository Structure
controller_code/ → Arduino code for controller node
target_code/ → Arduino code for receiver node
circuit_diagrams/ → Hardware wiring diagrams
images/ → Project images and architecture diagrams
README.md → Project documentation

## Full Tutorial

For a **detailed explanation of circuit wiring, code walkthrough, and project testing**, refer to the complete tutorial:

https://playwithcircuit.com/long-range-appliance-control-from-smartphone-using-arduino/

---

## License

This project is shared for educational and research purposes.
