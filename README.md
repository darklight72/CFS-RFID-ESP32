# CFS RFID — ESP32-C6 Reader & Writer

**Read and write Creality CFS-compatible filament RFID tags using a small, standalone device with a screen and web interface.** You can also manage filament entries and manually transfer material data to and from a compatible Creality printer.

Built around an **ESP32-C6 development board**, an **RC522 RFID reader**, and a **1.54-inch ST7789 colour display**. The **Seeed Studio XIAO ESP32-C6** is the tested reference hardware; other ESP32-C6 boards can use the [configurable generic Arduino sketch](docs/GENERIC_ESP32C6.md).

> **Based on DnG-Crafts/K2-RFID:** This implementation adds an ESP32-C6-based single-board design (developed and tested on the XIAO), TFT interface, updated web UI and printer integration. The original Arduino project is credited to **[DnG-Crafts/K2-RFID](https://github.com/DnG-Crafts/K2-RFID)**.

> **Security warning — local network only:** This firmware is intended for use on a **trusted local network**, not the public internet. The web interface (port **80**) and printer-transfer service (port **8081**) do not currently provide a verified authentication/access-control layer for sensitive operations. **Do not port-forward either service, place the ESP32 in a public-facing reverse proxy, or expose it to untrusted networks.** Anyone able to connect to the device may be able to alter configuration, initiate RFID operations or trigger printer database transfers. Use an isolated/trusted Wi-Fi network, change the default AP password, and keep printer credentials private. See [Security notes](docs/SECURITY.md).

> **Printer SSH password compatibility ([Issue #2](https://github.com/gavrd7/CFS-RFID-ESP32/issues/2)):** A contributor reported LibSSH-ESP32 authentication failures with a backslash (`\`) in the printer root password. However, a separate K2 test successfully connected and pulled the material database with a trailing backslash. This is **not a universal restriction**. See the [installation guide](docs/INSTALL.md#4-open-and-configure-the-firmware) for C++ escaping and troubleshooting.

![Finished CFS RFID device](images/rfid-printer.JPG)

> **OTA-capable firmware (successor to v1.0.1).** Adds manual browser firmware updates and CI-built release downloads. **Upgrading from v1.0.1 or earlier requires a one-time USB flash with new partitions and can erase the reader’s local database and settings.** See the migration instructions below.

## What can it do?

- **Scan a tag:** Hold a compatible RFID tag near the reader to see its details on the display.
- **Read or write on demand:** Open the web page and choose the action you want. The device **does not write tags automatically**.
- **Choose filament details:** Select an existing filament or create a custom entry using the web interface.
- **See information on the screen:** The TFT shows material names, filament colours, tag status and confirmations.
- **Sync with a printer:** Use the web interface to manually download material information from your printer or send updated entries back to it.
- **Run from one board:** The reader, screen, web interface and printer connection all run on an ESP32-C6 board. The XIAO is hardware-tested; [generic ESP32-C6 builds](docs/GENERIC_ESP32C6.md) are experimental.

### A look at the web interface

| Main screen | Reading a tag |
| --- | --- |
| ![Web interface home screen](screenshots/web-ui-home.PNG) | ![Tag read details in the web interface](screenshots/web-ui-tag-read.PNG) |

You can also manage filament records and review printer transfers:

| Material database | Printer sync |
| --- | --- |
| ![Filament material database](screenshots/web-ui-material-database.PNG) | ![Printer sync page](screenshots/web-ui-sync.PNG) |

## Other ESP32-C6 development boards (community / experimental)

The **Seeed XIAO ESP32-C6 remains the officially tested reference board**. Other ESP32-C6 variants may work with suitable SPI/GPIO wiring, flash capacity, and board-specific boot/antenna handling, but they are **not yet hardware verified**.

Releases provide **precompiled OTA firmware for the tested XIAO board only**. For other boards, download **`CFS-RFID-ESP32C6-Generic-Source-<version>.zip`**, which contains an editable Arduino sketch with the generic profile enabled and **the tested XIAO GPIO assignments pre-populated**. Keep those defaults if your board exposes the same GPIO numbers and the peripherals are wired accordingly; otherwise adjust `board_config.h` using the pin-mapping guide. Verify your board's reserved pins, flash capacity and partition compatibility, then compile and upload via USB. **Do not flash the XIAO OTA `.bin` to another board.** See [Generic ESP32-C6 setup](docs/GENERIC_ESP32C6.md).

## What you'll need

- An **ESP32-C6 development board** — **Seeed Studio XIAO ESP32-C6** (tested reference), or another ESP32-C6 board using the [generic source package](docs/GENERIC_ESP32C6.md) (experimental)
- An **MFRC522 / RC522** RFID reader
- A **1.54-inch ST7789 240 × 240 SPI TFT** screen
- Suitable wires, a USB cable and a power source
- **MIFARE Classic 1K** RFID tags for CFS-compatible read/write testing
- A computer with **Arduino IDE** to upload the firmware
- A Wi-Fi connection for the web interface and optional compatible Creality printer connection

A buzzer and printable enclosure are also included in this build.

For the pin-by-pin connections, see the **[wiring reference (PDF)](docs/Wiring_Reference.pdf)** or the [editable Word version](docs/Wiring_Reference.docx).

## Quick installation

This is the short version for getting started. **Follow the [full installation guide](docs/INSTALL.md)** for library requirements, wiring and first-time setup.

1. **Assemble the hardware.** Connect the ESP32-C6 board, RC522 and TFT. The [wiring reference](docs/Wiring_Reference.pdf) shows the tested XIAO connections; for another board, compare the GPIO numbers against its pinout and follow the [generic board guide](docs/GENERIC_ESP32C6.md). Check the display module's supply and backlight requirements.
2. **Install Arduino IDE and the ESP32 board package.** Install the external libraries listed in the [install guide](docs/INSTALL.md).
3. **Open the sketch:** `firmware/CFS_RFID_XIAO_C6_TFT/CFS_RFID_XIAO_C6_TFT.ino` for the XIAO, or the same sketch folder from the **Generic Source** release ZIP for another ESP32-C6 board. The sketch filename retains its original XIAO name for compatibility.
4. **Enter your own settings.** Review the placeholder Wi-Fi, access point and printer settings in the sketch. Do not put private passwords into a public GitHub commit.
5. **Select the board and upload.** For the tested XIAO, select **XIAO_ESP32C6**, **4 MB flash** and **USB CDC On Boot: Enabled** in Arduino IDE. For another ESP32-C6 board, select its matching board definition and verify the generic `board_config.h` GPIOs. The supplied **`partitions.csv` dual-OTA layout is designed for 4 MB flash**; confirm compatibility before flashing. Upload over USB first (back up existing reader data). Subsequent OTA uploads must use a firmware image **built for that exact board and partition layout**; the published XIAO `.bin` is for XIAO only.
6. **Connect and test.** Open Serial Monitor at **115200 baud**, connect to the device using its configured network settings, and open the web interface. Start with a **read test** on a known tag before attempting to write.

**Important:** If you're updating an already configured board, keep **Erase All Flash disabled** to avoid unnecessarily wiping stored settings. See the installation guide for LittleFS considerations.

## Reading and writing tags

1. Open the device's web interface.
2. For a **read**, select **Read Tag**, then hold a tag against the RC522 reader.
3. For a **write**, choose the filament details first, select **Write Tag**, then present a *spare/test* tag.
4. Check the confirmation on the TFT and in the browser.

Use an expendable tag when first learning the write process. Writing incorrect data may make a tag unsuitable for its intended use.

![Read tag prompt shown on the TFT](images/tag-read.JPG)

## Web firmware updates (OTA)

> **IMPORTANT — FIRST-TIME OTA MIGRATION MAY ERASE LOCAL DATA:** Firmware v1.0.1 and earlier use **No OTA** partitions. This release moves LittleFS to a different flash address/size to make room for two firmware slots. **Before flashing over USB, use Database Updater → Download Database to save `material_database.json`, and record your AP, Wi-Fi and printer settings.** The initial USB flash may leave the existing filesystem inaccessible or trigger formatting. After flashing, restore the database using **Select file / Update Database** or manually **Sync From Printer** (which may omit reader-only custom records). **The OTA `.bin` cannot perform this initial partition migration.** Full steps: [installation guide](docs/INSTALL.md#moving-an-existing-reader-to-web-firmware-updates).

**Printer SSH credentials after OTA:** Precompiled GitHub Actions binaries do not contain your private printer root password. Enter it once under **Settings → Printer SSH Password** and save. It is stored in the reader’s LittleFS `/config.ini` and was confirmed to survive a subsequent OTA update, along with the local material database. A blank password field retains the saved password; the Settings page does not reveal it. If you previously compiled the password into the sketch, you must enter it again once after upgrading to this release. Credentials are stored as plaintext and the settings interface uses unauthenticated HTTP; use a trusted, isolated LAN only.


On the **tested XIAO**, once the **dual-OTA partition layout** is installed over USB, **Settings → Firmware Updater** accepts the compatible **`CFS-RFID-XIAO-ESP32C6-OTA.bin`** application image from a successful GitHub Actions build or tagged release. On other ESP32-C6 boards, build firmware for the specific board; **do not use the published XIAO OTA binary**. These *subsequent* OTA updates normally leave the separate LittleFS data partition untouched. Hardware tests confirmed successful OTA flashing/reboot, printer reconnection, custom-tag reading, database restoration by printer sync and password persistence across updates. The release workflow packages the **XIAO OTA binary**, XIAO Arduino sketch ZIP, separate **generic ESP32-C6 source-only ZIP**, XIAO wiring PDF, XIAO enclosure files and SHA-256 checksums.

**USB recovery remains essential:** Web OTA cannot change the partition table, and there is no guaranteed automatic rollback. See the [OTA migration and upload instructions](docs/INSTALL.md#moving-an-existing-reader-to-web-firmware-updates).

Firmware uploads are manual; there is no automatic download from GitHub. **The web update endpoint has no verified authentication or signed-image enforcement; keep the device on a trusted local network only.**

## Printer sync and push

The web interface also has separate actions to **sync/download material data from the printer** and **push/upload entries to the printer**.

**Database Updater:** Use **Download Database** to back up the current local `material_database.json`. Use **Select file → Update Database** to restore that JSON, or **Sync From Printer** to retrieve the configured printer’s current material database. Printer sync may not recover custom material entries stored only on the reader. All actions are manual.

**Firmware updates:** The old unsafe updater was removed in v1.0.1; this release introduces the replacement OTA updater. First-time migration from No OTA still requires USB. Thereafter, manually upload a **board-compatible** OTA application binary via the web interface; published precompiled OTA images are for the tested XIAO only.

**These are manual actions, not automatic background synchronisation.** Back up the printer's material database before your first push, and avoid testing database writes on a production printer until you understand the recovery process.

![Printer push interface](screenshots/web-ui-push.PNG)

## 3D-printable enclosure

You can print the same case used for the **tested XIAO build**. The files are in **[enclosure_stls/](enclosure_stls/)**, including individual STL parts and a ready-to-print 3MF project. **The provided enclosure is designed for the XIAO; other ESP32-C6 development boards may need a different enclosure or mounting adaptation.**

![Finished reader enclosure](images/home-screen.JPG)

### Which file is which?

| File | What it's for |
| --- | --- |
| [RFID with display - front.stl](enclosure_stls/RFID%20with%20display%20-%20front.stl) | Main front shell. Holds the screen and RFID reader. |
| [RFID with display - display secure.stl](enclosure_stls/RFID%20with%20display%20-%20display%20secure.stl) | Display retainer, which presses onto the locating pegs. |
| [RFID with display - back.stl](enclosure_stls/RFID%20with%20display%20-%20back.stl) | Back cover designed to hold the tested XIAO ESP32-C6 board. |
| [RFID with display - xiao secure.stl](enclosure_stls/RFID%20with%20display%20-%20xiao%20secure.stl) | Optional XIAO retainer, fixed with two M2 screws. |
| [RFID with display - text.stl](enclosure_stls/RFID%20with%20display%20-%20text.stl) | Separate face decoration / lettering for the front. |
| [RFID with display.3mf](enclosure_stls/RFID%20with%20display.3mf) | Pre-assembled print project with colours and print settings already configured. |

### Putting the case together

1. **Place the front shell face down** on a clean, flat surface so the inside is accessible.
2. **Fit the TFT display and RFID reader** into their positions inside the front shell.
3. **Secure the screen:** line up the *display secure* retainer with its pegs, then push it down to hold the display in place. Check that nothing is trapped or under strain.
4. **For the XIAO enclosure, install the XIAO ESP32-C6 into the back cover.** If it needs additional support, fit the optional *xiao secure* part with **two M2 screws** in the pre-made holes. For another ESP32-C6 board, use a suitably adapted mount rather than assuming it fits the supplied case.
5. **Connect the electronics** according to the [wiring guide](docs/Wiring_Reference.pdf), carefully route the wires, then bring the front and back together.

**Adding the front logos:** The front STL does **not** include the RFID or Creality logos by default. To include them, import the front and the relevant separate logo/text pieces into **Creality Print** or **Orca Slicer**, align them and use the slicer's **Assemble** option to combine them into one print. Check alignment in the slicer preview before printing.

**Easiest way to print:** Open **[RFID with display.3mf](enclosure_stls/RFID%20with%20display.3mf)** to start with the pre-assembled model, colours and print settings. Review the printer profile, materials and plate settings before slicing for your own machine.

## Guides and reference

- **[Full installation guide](docs/INSTALL.md)** — software dependencies, Arduino settings and first tests for the tested XIAO
- **[Generic ESP32-C6 board guide](docs/GENERIC_ESP32C6.md)** — configurable GPIO mapping and source-only installation for other development boards
- **[Wiring reference](docs/Wiring_Reference.pdf)** — tested XIAO board-to-board connections
- **[Security notes](docs/SECURITY.md)** — credentials, network access and safe printer testing
- **[Project origin and licensing](docs/UPSTREAM_AND_LICENSE.md)** — upstream credits and outstanding permissions
- **[Firmware change history](docs/history/)** — earlier development notes

## Credits and licence status

**Licensing:** [AGPL-3.0-only](LICENSE) applies **only to original contributions that this project's author owns** (including original documentation and enclosure designs). It does **not** cover inherited DnG-Crafts Arduino code, bundled third-party sources or Creality logos. See [LICENSE-SCOPE.md](LICENSE-SCOPE.md) for the exact boundaries and [upstream licensing notes](docs/UPSTREAM_AND_LICENSE.md) for outstanding permissions. **Do not assume the complete firmware is AGPL-licensed.**

*This is a community hardware/firmware project, not an official Creality product.*
