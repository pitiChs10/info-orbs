# KY-022 receiver diagnostic

Standalone PlatformIO project for the Info Orbs ESP32 DevKit. It does not modify the main firmware project.

## Wiring (power disconnected)

- KY-022 VCC/+ -> ESP32 3V3
- KY-022 GND/- -> ESP32 GND
- KY-022 S/OUT -> ESP32 GPIO22

Identify the module labels before connecting; this list is not the physical pin order. Use a module rated for 3.3 V. Mount the receiver where the remote can see it.

## Run

Open this directory as a PlatformIO project, then Build, Upload and Monitor (115200 baud). Alternatively, from the repository root:

```powershell
pio run -d diagnostics/ir-receiver
pio run -d diagnostics/ir-receiver -t upload
pio device monitor -b 115200
```

If multiple serial devices are connected, add `--upload-port COMx` to upload and `-p COMx` to monitor.

Uploading replaces the running Orbs firmware. This test does not initialize the displays or Wi-Fi; upload the normal firmware again afterwards to restore Orbs operation.

Press the ESP32 EN/reset button with the monitor open to see the startup message. Aim an IR remote at the receiver and press a button briefly. Each decoded frame prints its protocol, address, command and raw value. Holding a button may produce repeat frames. No remote button codes are hardcoded.

An `Alive` line appears every two seconds. This proves the program is running, not that the receiver is connected: an unplugged input may also read HIGH. Recognized frames that track button presses confirm reception. UNKNOWN frames may mean an unsupported protocol or interference. If no frames arrive, check wiring, common ground, remote battery and line of sight. A continuously LOW input merits checking the module pinout and supply.

Version 2 also prints edges/2s and total edges, counted by a GPIO interrupt independently of the IR decoder. Zero edges during button presses means no electrical transitions were observed on GPIO22. Edges correlated with button presses but no decoded frames point toward protocol/timing issues; edges alone do not prove valid IR reception. Check for noise if transitions occur without pressing buttons.
