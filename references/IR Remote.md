# IR remote control

Open `info-orbs.code-workspace` for the complete Orbs firmware. The diagnostic workspace builds only the standalone receiver test.

## Wiring the KY-022 receiver

Disconnect power before wiring. Use the labels on your actual module; the table is not the physical left-to-right pin order.

| KY-022 module | ESP32 / Info Orbs |
| --- | --- |
| VCC / + | 3V3 |
| GND / - | GND |
| S / OUT (signal) | GPIO22, marked IO22 or G22 |

```text
KY-022                       ESP32 / Info Orbs
VCC / + -------------------- 3V3
GND / - -------------------- GND
S / OUT -------------------- GPIO22
```

The receiver used for this project was tested successfully at 3.3 V. Do not connect a 5 V signal directly to an ESP32 GPIO. Aim the remote at the receiver with a clear line of sight. A separate IR handheld remote is required; the KY-022 is only the receiver.

If you choose another free GPIO, also change `IR_REMOTE_PIN` in [firmware/config/config.h](../firmware/config/config.h). GPIO22 is a GPIO label, not physical connector position 22.

IR is enabled by default. In `firmware/config/config.h`, set `IR_REMOTE_ENABLED` to 0 to disable it, or override `IR_REMOTE_PIN` and `IR_REMOTE_ADDRESS`.

## Reading the button codes in Serial Monitor

1. Open [info-orbs.code-workspace](../info-orbs.code-workspace) in VS Code with PlatformIO. This selects the full firmware, including IR logging.
2. If `firmware/config/config.h` does not exist, copy [config.h.template](../firmware/config/config.h.template) to that name. Keep an existing configuration and adjust its settings rather than overwriting it.
3. Ensure the following settings are present in `config.h` (edit existing definitions; do not duplicate them):

   ```cpp
   #define IR_REMOTE_ENABLED 1
   #define IR_REMOTE_PIN 22
   #define IR_REMOTE_ADDRESS 0x00
   ```

4. Run **PlatformIO: Upload**, selecting the serial port of your ESP32. Close any other serial monitor if the port is busy.
5. Open **PlatformIO: Serial Monitor** on the same port at **115200 baud**. Press EN/reset if you want to see startup messages, and wait for the receiver-ready message. Widget actions become available after Wi-Fi and initial data loading.
6. Point the remote at the receiver and press each key briefly. Record the key label and its `Address` and `Command`. Avoid holding the key while recording, because it produces repeat frames.

Example output for the left arrow on the tested remote:

```text
Protocol=NEC Address=0x0 Command=0x44 Raw-Data=0xBB44FF00 32 bits LSB first
```

Use **`Command=0x44`** for the button mapping and **`Address=0x0`** for `IR_REMOTE_ADDRESS`. Do not use the full `Raw-Data` value as the command. `0xC` and `0x0C` are the same hexadecimal value.

Logging happens in `readRemote()` in [firmware/src/IRRemote.cpp](../firmware/src/IRRemote.cpp), before the address and command filters:

```cpp
IrReceiver.printIRResultShort(&Serial);
```

Therefore, decoded NEC keys are printed even when their command or address is not yet mapped. The existing mapped arrow and OK keys still perform their actions while being logged.

From a PlatformIO terminal at the repository root, the equivalent commands are:

```powershell
pio device list
pio run -t upload --upload-port COM5
pio device monitor -p COM5 -b 115200
```

Replace `COM5` with your device's port; COM5 is only an example. Exit the monitor with Ctrl+C before another upload.

The full firmware currently enables only the NEC decoder. A remote using another protocol requires a decoder/filter change, not just new command numbers. For initial wiring checks or protocol discovery, open [ir-receiver-test.code-workspace](../ir-receiver-test.code-workspace) and follow the [standalone diagnostic instructions](../diagnostics/ir-receiver/README.md). Uploading that test replaces the running Orbs firmware until you upload the full firmware again.

## Changing the button codes

Edit **`switch (command)` inside `readRemote()`** in [firmware/src/IRRemote.cpp](../firmware/src/IRRemote.cpp). Replace the hexadecimal value after `case` for each key, keeping the intended action:

```cpp
case 0x44: return RemoteAction::Previous; // Left arrow
case 0x43: return RemoteAction::Next;     // Right arrow
case 0x40: return RemoteAction::OK;       // Center / OK
case 0x16: return RemoteAction::Digit0;   // Number 0
case 0x0C: return RemoteAction::Digit1;   // Number 1
// Digit2 through Digit9 are immediately below these in the same switch.
```

For example, if your left arrow prints `Command=0x10`, change only its mapping to:

```cpp
case 0x10: return RemoteAction::Previous;
```

Use the remaining number-key entries to map all ten digits. Every `case` value must be unique; two keys with the same command cannot be assigned different actions with this mapping. The tables below show the supplied defaults, which may differ from your remote.

If your remote prints a different NEC address, set `IR_REMOTE_ADDRESS` in `firmware/config/config.h` to that value. The current implementation accepts one address. GPIO, address and enable settings belong in `config.h`; button command values belong in `IRRemote.cpp`.

After editing, **Build and Upload the full firmware again** from `info-orbs.code-workspace`. Changes in source files do not affect firmware already on the device. No changes to the IRremote library, image assets or diagnostic sketch are needed to remap buttons.

## Default navigation buttons
| NEC address | Command | Action |
| --- | --- | --- |
| 0x00 | 0x44 (Left) | Previous widget |
| 0x00 | 0x43 (Right) | Next widget |
| 0x00 | 0x40 (OK) | Current widget's middle-button short press |

On the clock, OK changes clock style. Physical buttons remain operational. Repeat frames from held buttons, other addresses, unknown commands and invalid frames are ignored. Each accepted action restarts the automatic widget-cycle timer. Remote long presses are not implemented.

Control is available after Wi-Fi and initial widget data loading. Existing blocking network requests/rendering may delay or miss presses; this change does not make the firmware asynchronous.

Before uploading, configure `firmware/config/config.h` using the supplied template. This merged project keeps browser-based configuration. Saved settings take precedence over template defaults; do not copy a config.h from the older IR-only project.

From the repository root, use `pio run` to build and `pio run -t upload` to upload the complete firmware. Open Serial Monitor at 115200 baud. Decoded frames print Protocol, Address and Command. Navigation uses the same MainHelper handler as the physical buttons; color selections print `IR digit ...`.

Hardware check: press Left and Right separately and check widget changes; press OK on the clock and check its style; hold each button and check it does not repeatedly cycle; check the physical buttons still work.
## Clock color presets

For the clock, number keys apply only to normal (font-rendered) mode. Nixie and custom image clocks ignore them entirely. Their image rendering and overlays retain the configured colors. Matrix also supports presets; other widgets ignore them.

| Key | NEC command | Clock color |
| --- | --- | --- |
| 0 | 0x16 | Restore configured clock colors (including saved web settings) |
| 1 | 0x0C | Green |
| 2 | 0x18 | Red |
| 3 | 0x5E | Blue |
| 4 | 0x08 | Cyan |
| 5 | 0x1C | Purple |
| 6 | 0x5A | Yellow |
| 7 | 0x42 | Orange |
| 8 | 0x52 | Pink |
| 9 | 0x4A | White |

Inactive segments use a darker version of the selected color. The colon, seconds indicator and AM/PM use the selected color in normal mode. Selection survives widget/style changes during runtime, but resets on reboot. Number keys in image modes do not change the stored normal-clock preset.

Matrix retains the custom-image branch's web settings and rendering API. It is enabled by default (`INCLUDE_MATRIXSCREEN WIDGET_ON`); saved web settings can override this. Key 0 restores the configured trail and leading-character colors (blue/cyan by default). Keys 1-9 select dark green/green, dark red/red, blue/cyan, cyan/blue, purple/pink, orange/yellow, red/orange, pink/purple and gray/white, respectively. Remote presets are temporary overrides and do not overwrite the stored web configuration. Clock and Matrix selections are independent.

Custom Images retains browser uploads and multiple pages. OK advances to the next image page, arrows change widgets, and number keys do not recolor images. See [Combined project setup](Combined%20Project.md) before flashing, especially the LittleFS upload behavior.
