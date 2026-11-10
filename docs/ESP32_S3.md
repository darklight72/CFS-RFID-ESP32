# ESP32-S3 Super Mini / Zero — Arduino source installation

ESP32-S3 support was contributed and hardware-tested by **[darklight72](https://github.com/darklight72)** in [pull request #6](https://github.com/gavrd7/CFS-RFID-ESP32/pull/6). The contributor tested generic **ESP32-S3 Super Mini** and **ESP32-S3-Zero** development boards with an RC522 RFID reader and 1.54-inch ST7789 TFT.

The contributor reports successful **RFID reading and writing, printer material database download and upload, OTA updates, network access, and TFT screen operation** on both boards. This is contributor-reported testing; the project maintainer's physically verified reference hardware remains the **Seeed XIAO ESP32-C6**. The source-only package does not imply every ESP32-S3 board or flash variant has been tested.

**No precompiled S3 firmware is provided.** Build and upload the sketch for your own board using Arduino IDE.

## Connections used for hardware testing

Both devices share the same SPI clock and MOSI signals. Keep **separate chip-select pins** for the RC522 and TFT.

| ESP32-S3 pin | Connected components |
| --- | --- |
| 3V3 | RC522 3.3V; TFT VCC; TFT BLK (only if your display module supports this connection) |
| GND | RC522 GND; TFT GND; optional buzzer GND |
| GPIO12 | RC522 SCK; TFT SCL |
| GPIO11 | RC522 MOSI; TFT SDA |
| GPIO13 | RC522 MISO |
| GPIO10 | RC522 SDA / SS / CS |
| GPIO6 | RC522 RST |
| GPIO2 | TFT CS |
| GPIO4 | TFT DC |
| GPIO5 | TFT RST |
| GPIO1 | Optional buzzer signal |

These GPIO assignments are pre-populated in `board_config.h` under `CFS_BOARD_S3_MINI_ZERO`. The source uses the ESP32-S3 architecture selected in Arduino IDE to activate that profile automatically.

**Wiring diagram:** [view the contributor's original diagram and detailed test report](https://github.com/gavrd7/CFS-RFID-ESP32/pull/6#issuecomment-6758449460).

Use **3.3V logic** for the RC522. Check the actual TFT module's VCC and backlight specifications before connecting BLK to 3V3. Verify your development board's pin labels and onboard peripherals against its own schematic.

## Arduino IDE setup

1. Download and extract **`CFS-RFID-ESP32S3-SuperMini-Zero-Source-<version>.zip`** from a release containing ESP32-S3 support. Keep the full `CFS_RFID_XIAO_C6_TFT` sketch directory and `partitions.csv` together. The folder name is retained for compatibility; the sketch now supports both C6 and S3.
2. Open `CFS_RFID_XIAO_C6_TFT.ino` in Arduino IDE and install the ESP32 board platform and libraries listed in [INSTALL.md](INSTALL.md).
3. Select an **ESP32-S3** target (not ESP32-C6). The contributor reports testing **Waveshare ESP32-S3-Zero** and **ESP32S3 Dev Module** board definitions. Choose the definition appropriate to your physical board and its flash capacity.
4. Review the S3 GPIO table above and the values in `board_config.h`. The Super Mini/Zero mapping is already filled in and normally needs no edits for the contributor's wiring.
5. For the **tested 4MB-flash boards**, select the supplied **Custom** `partitions.csv` layout with dual OTA slots. Check that your board's flash capacity, flash mode and selected partition scheme match; other flash configurations may require adaptation.
6. Compile and **upload over USB**. Open Serial Monitor at **115200 baud**, then configure Wi-Fi and the printer SSH password through the firmware's web Settings interface.
7. Test TFT output, RFID reading and writing using a spare tag, and printer material database **read** operations before considering any printer database write. Keep backups and a USB recovery path.

### Contributor's reported tool settings

| Arduino option | Reported setting |
| --- | --- |
| USB CDC On Boot | Enabled |
| CPU Frequency | 240 MHz (WiFi) |
| Core Debug Level | None |
| USB DFU On Boot | Disabled |
| Flash Mode | QIO 80 MHz |
| Partition Scheme | Custom (use sketch's `partitions.csv`) |
| PSRAM | Disabled |
| Events Run On | Core 1 |
| Arduino Runs On | Core 1 |
| USB Firmware MSC On Boot | Disabled |
| Upload Mode | UART0 / Hardware CDC |
| Upload Speed | 115200 |
| USB Mode | Hardware CDC and JTAG |
| Erase All Flash Before Sketch Upload | Disabled for subsequent builds |

The contributor used **Erase All Flash Before Sketch Upload: Enabled** for their initial flash. **This erases stored data and configuration.** It is not generally required for an already working board with the correct partition layout. If migrating from an older layout, first back up the local material database and settings using the procedures in [INSTALL.md](INSTALL.md). The sketch calls `LittleFS.begin(true)`, which can format an unmountable filesystem.

## Firmware updates and releases

The published `CFS-RFID-XIAO-ESP32C6-OTA.bin` is **only for the Seeed XIAO ESP32-C6**. **Never install it on an ESP32-S3.** For an S3, compile firmware specifically for the selected board and installed partition layout. Although OTA has been tested by the contributor, keep USB recovery available.

Printer SSH/SCP and material database transfer logic are shared with the C6 build and have not been modified for this hardware addition. Printer database upload remains a manual operation; create backups before using it.

## Attribution

**ESP32-S3 support and hardware validation:** [darklight72](https://github.com/darklight72), [PR #6](https://github.com/gavrd7/CFS-RFID-ESP32/pull/6) — including tested wiring, board configuration, RFID, printer-transfer and OTA results. The original project's upstream acknowledgements and licence boundaries remain documented in [UPSTREAM_AND_LICENSE.md](UPSTREAM_AND_LICENSE.md).
