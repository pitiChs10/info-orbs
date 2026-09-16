# Custom Images + IR remote

This separate project merges `custom-image-widget` and `feature/ir-remote` on branch `feature/customimage-ir`. It uses the newer Custom Images firmware architecture, with its configuration portal, browser image uploads, image pages, LittleFS layout and task manager. The original source branches and local projects are unchanged.

## Open and build

Open `info-orbs.code-workspace` in this directory. Use PlatformIO environment **infoorbs-v0_3** for the standard board. The diagnostic workspace remains a separate receiver-only test.

```powershell
pio run -e infoorbs-v0_3
pio run -e infoorbs-v0_3 -t buildfs
```

Configuration is optional: copy `firmware/config/config.h.template` to `config.h` if needed. Do not copy the old IR-only project's config.h into this newer firmware. Defaults enable Custom Images and Matrix, and the widget capacity is 10. Existing saved settings may still disable these widgets: enable them in the web configuration portal and restart.

## Included behavior

- Left (NEC 0x44) and Right (0x43): previous/next widget.
- OK (0x40): the current widget's middle-button short press. In Custom Images this advances the image page; in Clock it changes clock style.
- Numbers 0-9: color presets for the active normal Clock or Matrix.
- Nixie, custom clock images and Custom Images pages ignore number-key presets.
- Key 0 removes the remote override and restores the colors configured in the web portal or firmware defaults. Presets survive widget switches, but reset on reboot and are not written to saved settings.
- Physical buttons, browser controls and existing browser image uploads remain available.

See [IR wiring, codes and Serial Monitor](IR%20Remote.md) and [Custom Images instructions](../images/CustomImageWidget/README.md).

## Upload and images

This project keeps the custom-image branch's partition layout and upload scripts. Its normal PlatformIO **Upload** runs a filesystem upload before the firmware upload. This replaces the device's LittleFS contents with locally built files, including the bundled sample images. Download/back up personal images from the web portal before flashing, then restore them through Custom Images afterwards. Switching from the old IR firmware also changes the filesystem layout.

Build and buildfs alone do not write anything to the device. No device upload is performed as part of preparing this project.

## Device verification

1. Upload using the correct board environment, then configure Wi-Fi and enable Custom Images/Matrix in the web portal if saved settings have them off.
2. Upload two complete five-image pages through Custom Images. Use OK to advance between them and arrows to leave/return to the widget.
3. On the normal clock, test colors 1-9 and restore with 0. Switch to Nixie/custom clock and confirm number keys do not change images.
4. On Matrix, test both colors and return with 0 to web-configured colors; switch away and back repeatedly.
5. Check the physical buttons and confirm each held IR key produces only one action.

The firmware and LittleFS image are build-checked; combined behavior still needs this hardware verification.
