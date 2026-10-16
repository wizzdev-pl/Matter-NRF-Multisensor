# WizzDev Matter nRF Multisensor

## Overview
This repository contains the WizzDev Matter nRF Multisensor, an open-source family of multi-function peripherals based on [Matter](https://csa-iot.org/all-solutions/matter/) over [Thread](https://www.threadgroup.org/) and Nordic Semiconductor nRF microcontrollers. The project uses one shared, modular hardware and software architecture, so every functional variant integrates with a smart home ecosystem the same way: it is commissioned once and then shows up as a set of Matter endpoints in the ecosystem's app.

The project serves two purposes:

- **Client demonstrations:** a working device that shows what we can build with Matter, Thread and nRF
- **New hire training:** a readable codebase that shows how a Matter device is put together end to end: the Matter data model (endpoints, device types and clusters generated with ZAP), Thread networking and commissioning, and a hexagonal (ports & adapters) application architecture

The planned modules are:

- **Sensors and buttons:** buttons exposed as Contact Sensor endpoints, and temperature sensors
- **E-paper display:** an e-paper screen showing the device state in real time
- **Light sensor and lighting control:** an ambient light sensor that controls an LED automatically based on the surrounding brightness
- **Ventilation and fan speed control:** PWM fan control, with an encoder for manual speed adjustment
- **Thermostat:** a dedicated thermostat endpoint, configured with the encoder, driving a heating relay through a control algorithm
- **Visual signalling and actuator:** multi-colour RGB LEDs and a relay (On/Off Plug-in Unit), each with its own Matter endpoint

| Module                            | Matter device type                                          | Status                                                                   |
| --------------------------------- | ----------------------------------------------------------- | ------------------------------------------------------------------------ |
| Sensors and buttons               | Contact Sensor (0x0015), Temperature Sensor (0x0302)        | Contact sensors done (simulated by dev kit buttons), temperature planned |
| E-paper display                   | none (local display)                                        | Planned                                                                  |
| Light sensor and lighting control | Light Sensor (0x0106)                                       | Planned                                                                  |
| Ventilation and fan speed control | Fan (0x002B)                                                | Planned                                                                  |
| Thermostat                        | Thermostat (0x0301)                                         | Planned                                                                  |
| Visual signalling and actuator    | Extended Color Light (0x010D), On/Off Plug-in Unit (0x010A) | Planned                                                                  |

## Prerequisites
Before building, install the following on your PC (the commands below were used on Linux):

- [nRF Connect SDK](https://www.nordicsemi.com/Products/Development-software/nrf-connect-sdk) **v3.4.0** with its toolchain, either through the [nRF Connect for VS Code extension](https://marketplace.visualstudio.com/items?itemName=nordic-semiconductor.nrf-connect-extension-pack) or as a `west` workspace
- a serial terminal, for example `minicom`

Make sure you have access to the required hardware:

- nRF54L15 DK (`nrf54l15dk/nrf54l15/cpuapp`, the only supported board)
- USB cable for the DK
- a Matter-over-Thread smart home hub that also acts as a Thread Border Router (for example Apple TV 4K or HomePod mini, a Google Nest hub, or an Amazon Echo with Thread support)
- a phone with the matching app (Apple Home, Google Home or Amazon Alexa)


## Building and flashing
For Linux users a prerequisite to flash the device and open its serial console, is to add your user to the dialout group:

```
sudo usermod -a -G dialout $USER
```

This is a standard nRF Connect SDK / Zephyr sysbuild application. You can build and flash it either from the command line or with the nRF Connect for VS Code extension. Both use the same `west build` / `west flash` commands, so they produce the same firmware.

### **Command line:**
From an nRF Connect SDK west workspace:

```bash
west build -b nrf54l15dk/nrf54l15/cpuapp --sysbuild .
west flash
```

You can also use the `Makefile` wrapper: `make build` and `make flash`. `make static-analysis` runs clang-tidy over `src/` and needs a completed build first.

### **nRF Connect for VS Code extension:**
1. Open this project folder in VS Code with the nRF Connect for VS Code extension installed.
2. In the **nRF Connect** side panel, click **Add Build Configuration**, select the target board (`nrf54l15dk/nrf54l15/cpuapp`), keep sysbuild enabled, and build.
3. Connect the board over USB, then use **Flash** on the generated build configuration to program it.

The device name, vendor name and setup code are stored in the factory data partition, which is generated at build time. Changes to them only reach the device after a full flash.

---

## Commissioning

### **Getting the QR code:**
Every ecosystem commissions the device by scanning its Matter QR code. The device prints it on its serial console at boot. Connect to the console (the second serial port of the DK, e.g. `VCOM1`, usually `/dev/ttyACM2`):

```bash
minicom -D /dev/ttyACM2 -b 100000
```

If there are no logs, press the reset button on the board. Look for the QR code and manual pairing code in the boot log, for example:

```
I: [SVR]SetupQRCode: [MT:6FCJ142C00KA0648G00]
I: [SVR]Copy/paste the below URL in a browser to see the QR Code:
I: [SVR]https://project-chip.github.io/connectedhomeip/qrcode.html?data=MT%3A6FCJ142C00KA0648G00
I: [SVR]Manual pairing code: [34970112332]
```

Open that URL to display the QR code, or type the manual pairing code into the app instead. The setup passcode and discriminator behind them are set in `prj.conf` (`CONFIG_CHIP_DEVICE_SPAKE2_PASSCODE` / `CONFIG_CHIP_DEVICE_DISCRIMINATOR`).

If the device was commissioned before, factory-reset it first: hold Button 1 for 3 s, or type `matter device factoryreset` in the console.

### **Into Apple Home:**
1. Open the Home app on iPhone.
2. Tap **+** → **Add Accessory**.
3. Scan the QR code.
4. Apple Home commissions the device automatically.
5. Optionally rename the accessory and adjust its displayed device type.

### **Into Amazon Alexa:**
1. Open the Amazon Alexa app.
2. Tap **+** (top right) → **Device** → **Other** → **Matter**.
3. Scan the QR code.
4. Alexa commissions the device.

If commissioning fails, power-cycle the Alexa hub and retry. This sometimes takes multiple attempts.

### **Into Google Home:**
1. Open the Google Home app.
2. Tap **+** (top right) → **Device**.
3. Scan the QR code.
4. Google commissions the device.

If commissioning fails, power-cycle the Google hub and retry. This sometimes takes multiple attempts.

---

## Working with configured device
After commissioning, the device shows up in the ecosystem app as three contact sensors. No physical sensors are wired up yet, so the DK's buttons simulate them: press a button and the matching sensor changes between "Open" and "Closed" in the app.

| Control         | Action                                                       |
| --------------- | ------------------------------------------------------------ |
| Button 1 (hold) | Hold 3 s: Thread network reset (full Matter factory reset)   |
| Button 2        | Toggle contact sensor 1 (endpoint 1)                         |
| Button 3        | Toggle contact sensor 2 (endpoint 2)                         |
| Button 4        | Toggle contact sensor 3 (endpoint 3)                         |
| LED 1           | Matter/Thread connectivity status (Nordic sample board code) |
| LED 2           | Blinks while an endpoint is asked to identify itself         |

### Endpoints
Each contact sensor is its own Matter endpoint, with the Contact Sensor device type (0x0015) and a `BooleanState` cluster:

| Endpoint | Name (FixedLabel / default UserLabel `name`) | `StateValue = true` means | Button |
| -------: | -------------------------------------------- | ------------------------- | ------ |
|        1 | `Contact 1`                                  | contact closed            | 2      |
|        2 | `Contact 2`                                  | contact closed            | 3      |
|        3 | `Contact 3`                                  | contact closed            | 4      |

The sensors are independent of each other. All three start **open** at boot (`StateValue = false`), and sensor state is not persisted across reboots. Ecosystems such as Apple Home show `true` as "Closed" and `false` as "Open".

On first boot the device names each endpoint "Contact 1", "Contact 2" and "Contact 3", and the device itself "Matter multisensor". Names you set in the ecosystem app are kept.

---

## Project structure
The application uses a hexagonal (ports & adapters) architecture, which keeps the device logic independent of the hardware and of the Matter stack:

### TODO: add drawio graph

- `src/domain/`: `ESensorId`, `EContactState` and `ContactSensorsDomain`, which holds every sensor's state (all start `Open`). `toggle()` flips one sensor and returns its new state.
- `src/ports/`: the interfaces the adapters are written against:
  - `ContactSensorInputPort` (driving) and `ContactSensorObserver` (driven)
  - the `ThreadNetworkResetInputPort` / `ThreadNetworkResetOutputPort` pair for the button 1 reset
- `src/usecases/`:
  - `ToggleContactSensorUseCase` toggles a sensor in the domain and notifies every registered observer with the resulting state
  - `ResetThreadNetworkUseCase` relays the reset request
- `src/adapters/`:
  - `ButtonAdapter`: button presses stand in for sensor signals; it will be replaced by real sensors later
  - `MatterThreadAdapter`: owns the Matter/Thread stack and publishes each sensor on its endpoint
- `src/main.cpp` constructs everything above as locals of `main()` and wires them together.

New modules follow the same pattern: a domain, ports and a use case for the new kind of endpoint, plus adapters for its hardware and its Matter clusters.

### Matter data model
The data model lives in `matter_endpoint_zap/multisensor.zap`. The build uses the pre-generated `matter_endpoint_zap/zap-generated/` directory, so after changing the `.zap` file, regenerate it:

```bash
west zap-generate -z matter_endpoint_zap/multisensor.zap
```

### Thread role
The device is an always-on, router-capable Thread device (Full Thread Device). Its radio receiver stays on, so it answers controllers immediately and can act as a Thread router that extends the mesh for other devices. It is meant to be powered from USB or mains.

---

## Dependencies
This project is built on:

- [nRF Connect SDK](https://www.nordicsemi.com/Products/Development-software/nrf-connect-sdk)
- [Matter (connectedhomeip)](https://github.com/project-chip/connectedhomeip)
- [OpenThread](https://openthread.io/) (Thread networking)
