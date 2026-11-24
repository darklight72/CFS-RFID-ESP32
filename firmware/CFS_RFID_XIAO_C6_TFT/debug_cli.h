#pragma once

#ifdef DEBUG_CLI

#include "board_config.h"
extern MFRC522 mfrc522;
extern Adafruit_ST7789 tft;
extern void drawDisplay();
extern void cliRestoreNormalDisplay();
extern void screenStatus(const String &mode);
extern bool tempWatchEnabled;
extern uint32_t nextTempReport;
extern float maxObservedTemp;
extern long t_freq;
extern IPAddress Server_IP;
extern IPAddress Subnet_Mask;
extern String AP_SSID;
extern String AP_PASS;

#include <WiFi.h>
#include <esp_system.h>

// ---------------------------------------------------
// Fallback board / hardware definitions (#ifndef safe)
// ---------------------------------------------------

#ifndef BOARD_NAME
#  if defined(CONFIG_IDF_TARGET_ESP32S3)
#    define BOARD_NAME "ESP32-S3"
#  elif defined(CONFIG_IDF_TARGET_ESP32C6)
#    define BOARD_NAME "ESP32-C6"
#  else
#    define BOARD_NAME "ESP32"
#  endif
#endif

#ifndef FW_VERSION
#  define FW_VERSION "0.1.0-debug"
#endif

// ---------------------------------------------------
// LED configuration (#ifndef safe)
// ---------------------------------------------------

// Define BOARD_HAS_RGB as 1 in board_config.h for boards with an RGB LED
// (e.g. ESP32-S3 Zero). Leave 0 or undefined for standard/non-RGB LEDs.
#ifndef BOARD_HAS_RGB
#  define BOARD_HAS_RGB 0
#endif

#ifndef CLI_LED_PIN
#  ifdef LED_BUILTIN
#    define CLI_LED_PIN LED_BUILTIN
#  else
#    define CLI_LED_PIN -1
#  endif
#endif

#ifndef CLI_LED_ACTIVE_HIGH
#  define CLI_LED_ACTIVE_HIGH 1
#endif

#define CLI_RFID_OBJ mfrc522

// ST7789 display instance & geometry hooks
#define CLI_TFT_OBJ tft


// Function to reset RFID device

inline void cliDefaultRfidReset()
{
    CLI_RFID_OBJ.PCD_Reset();
    delay(5);
    CLI_RFID_OBJ.PCD_Init();
}

// Function to restore screen after tests

inline void cliDisplayRestore()
{
    cliRestoreNormalDisplay();
}

// ---------------------------------------------------
// Function prototypes
// ---------------------------------------------------

inline void cliHelp(const String& args);
inline void cliStatus(const String& args);
inline void cliWifi(const String& args);
inline void cliHeap(const String& args);
inline void cliFlash(const String& args);
inline void cliBuild(const String& args);
inline void cliTemp(const String& args);
inline void cliReboot(const String& args);
inline void cliRfid(const String& args);
inline void cliDisplay(const String& args);
inline void cliLed(const String& args);
inline void cliFreq(const String& args);
inline void cliAP(const String& args);

// ---------------------------------------------------
// Command table
// ---------------------------------------------------

struct CliCommand
{
    const char* name;
    const char* help;
    void (*handler)(const String& args);
};

inline CliCommand cliCommands[] =
{
    { "ap",      "AP status (settings managed via web)",                   cliAP},
    { "build",   "Build information",                        cliBuild   },
#if defined(CONFIG_IDF_TARGET_ESP32S3)
    { "cpu",     "CPU frequency [80|160|240]", cliFreq },
#else
    { "cpu",     "CPU frequency [80|160]", cliFreq },
#endif
    { "display", "Display status | test | clear | restore | color <name>",    cliDisplay },
    { "flash",   "Flash information",                        cliFlash   },
    { "heap",    "Heap statistics",                          cliHeap    },
    { "help",    "Show available commands",                  cliHelp    },
    #if BOARD_HAS_RGB
    { "led",     "LED on [r|g|b] | off | blink (R->G->B x5)", cliLed    },
    #else
    { "led",     "LED on | off | blink",                     cliLed     },
    #endif
    { "status",  "System status summary",                    cliStatus  },
    { "reboot",  "Restart ESP32",                            cliReboot  },
    { "rfid",    "RFID ver | status | reset | scan | dump",  cliRfid    },
    { "temp",    "MCU temperature watch [on|off|1-180]",                          cliTemp    },
    { "wifi",    "WiFi info | detail | power [8.5|11|20]",   cliWifi    },
        
};

constexpr size_t CLI_COMMAND_COUNT =
    sizeof(cliCommands) / sizeof(cliCommands[0]);

// ---------------------------------------------------
// Small argument helper
// ---------------------------------------------------

inline String cliPopToken(String& rest)
{
    rest.trim();
    if (rest.isEmpty())
    {
        return String();
    }

    int spaceIdx = rest.indexOf(' ');
    if (spaceIdx < 0)
    {
        String token = rest;
        rest = "";
        return token;
    }

    String token = rest.substring(0, spaceIdx);
    rest = rest.substring(spaceIdx + 1);
    rest.trim();
    return token;
}

// ---------------------------------------------------
// Dispatcher
// ---------------------------------------------------

inline void cliProcessCommand(const String& input)
{
    String cmd = input;
    cmd.trim();

    if (cmd.isEmpty())
    {
        return;
    }

    String args;
    int spacePos = cmd.indexOf(' ');

    if (spacePos >= 0)
    {
        args = cmd.substring(spacePos + 1);
        args.trim();
        cmd = cmd.substring(0, spacePos);
    }

    for (size_t i = 0; i < CLI_COMMAND_COUNT; i++)
    {
        if (cmd.equalsIgnoreCase(cliCommands[i].name))
        {
            cliCommands[i].handler(args);
            return;
        }
    }

    Serial.printf("Unknown command: %s\nType 'help'\n", cmd.c_str());
}

// ---------------------------------------------------
// Help
// ---------------------------------------------------

inline void cliHelp(const String&)
{
    Serial.println();
    Serial.println("Commands:");
    Serial.println("--------------------------------");

    for (size_t i = 0; i < CLI_COMMAND_COUNT; i++)
    {
        Serial.printf("%-10s %s\n", cliCommands[i].name, cliCommands[i].help);
    }

    Serial.println("--------------------------------");
}

// ---------------------------------------------------
// Status
// ---------------------------------------------------

inline void cliStatus(const String&)
{
    unsigned long totalSeconds = millis() / 1000UL;

unsigned long days =
    totalSeconds / 86400UL;

unsigned long hours =
    (totalSeconds % 86400UL) / 3600UL;

unsigned long minutes =
    (totalSeconds % 3600UL) / 60UL;

unsigned long seconds =
    totalSeconds % 60UL;

    Serial.println("\n===== STATUS =====");
    Serial.printf("Board    : %s\n", BOARD_NAME);
    Serial.printf("Version  : %s\n", FW_DISPLAY_VERSION);
    Serial.printf("Uptime   : %lud %luh %lum %lus\n",
    days,
    hours,
    minutes,
    seconds
    );
    Serial.printf("WiFi     : %s\n", WiFi.isConnected() ? "Connected" : "Disconnected");
    Serial.printf("IP       : %s\n", WiFi.localIP().toString().c_str());
    Serial.printf("RSSI     : %d dBm\n", WiFi.RSSI());
    Serial.printf("Heap     : %u bytes free\n", static_cast<unsigned int>(ESP.getFreeHeap()));
    Serial.println("==================");
}

// ---------------------------------------------------
// WiFi (collapsed: info / detail / power)
// ---------------------------------------------------

inline void cliWifiPrintBasic()
{
    Serial.println();
    Serial.printf("Status : %s\n", WiFi.isConnected() ? "Connected" : "Disconnected");
    Serial.printf("SSID   : %s\n", WiFi.SSID().c_str());
    Serial.printf("IP     : %s\n", WiFi.localIP().toString().c_str());
    Serial.printf("RSSI   : %d dBm\n", WiFi.RSSI());
    Serial.printf("MAC    : %s\n", WiFi.macAddress().c_str());
}

inline void cliWifiPrintDetail()
{
    int8_t rawPower = WiFi.getTxPower();
    Serial.println();
    Serial.printf("Status   : %s\n", WiFi.isConnected() ? "Connected" : "Disconnected");
    Serial.printf("SSID     : %s\n", WiFi.SSID().c_str());
    Serial.printf("BSSID    : %s\n", WiFi.BSSIDstr().c_str());
    Serial.printf("Channel  : %d\n", WiFi.channel());
    if (rawPower >= 0)
    {
        Serial.printf("Power    : %.2f dBm\n", static_cast<float>(rawPower) / 4.0f);
    }
    else
    {
        Serial.printf("Power    : unknown (%d)\n", rawPower);
    }
    Serial.printf("IP       : %s\n", WiFi.localIP().toString().c_str());
    Serial.printf("Gateway  : %s\n", WiFi.gatewayIP().toString().c_str());
    Serial.printf("Subnet   : %s\n", WiFi.subnetMask().toString().c_str());
    Serial.printf("DNS      : %s\n", WiFi.dnsIP().toString().c_str());
    Serial.printf("Hostname : %s\n", WiFi.getHostname());
}

inline void cliWifiSetPower(const String& level)
{
    if (level == "8.5")
    {
        Serial.println("Setting WiFi Power to 8.5 dBm");
        WiFi.setTxPower(WIFI_POWER_8_5dBm);
        return;
    }

    if (level == "11")
    {
        Serial.println("Setting WiFi Power to 11 dBm");
        WiFi.setTxPower(WIFI_POWER_11dBm);
        return;
    }

    if (level == "20")
    {
        Serial.println("Setting WiFi Power to 20 dBm");
        WiFi.setTxPower(WIFI_POWER_20dBm);
        return;
    }

    Serial.println("Usage: wifi power 8.5|11|20");
}

inline void cliWifi(const String& args)
{
    String rest = args;
    String sub = cliPopToken(rest);

    if (sub.isEmpty() || sub.equalsIgnoreCase("info"))
    {
        cliWifiPrintBasic();
        return;
    }

    if (sub.equalsIgnoreCase("detail"))
    {
        cliWifiPrintDetail();
        return;
    }

    if (sub.equalsIgnoreCase("power"))
    {
        if (rest.isEmpty())
        {
            int8_t rawPower = WiFi.getTxPower();
            if (rawPower >= 0)
            {
                Serial.printf("Current WiFi Power: %.2f dBm\n", static_cast<float>(rawPower) / 4.0f);
            }
            else
            {
                Serial.println("WiFi TX power unavailable");
            }
            Serial.println("Usage: wifi power 8.5|11|20");
            return;
        }

        cliWifiSetPower(rest);
        return;
    }

    Serial.println("Usage: wifi [info|detail|power <8.5|11|20>]");
}

// ---------------------------------------------------
// Heap
// ---------------------------------------------------

inline void cliHeap(const String&)
{
    Serial.println();
    Serial.printf("Free Heap     : %u\n", static_cast<unsigned int>(ESP.getFreeHeap()));
    Serial.printf("Minimum Heap  : %u\n", static_cast<unsigned int>(ESP.getMinFreeHeap()));
    Serial.printf("Heap Size     : %u\n", static_cast<unsigned int>(ESP.getHeapSize()));
    Serial.printf("Largest Block : %u\n", static_cast<unsigned int>(ESP.getMaxAllocHeap()));
}

// ---------------------------------------------------
// Flash
// ---------------------------------------------------

inline void cliFlash(const String&)
{
    Serial.println();
    Serial.printf("Flash Size        : %u\n", static_cast<unsigned int>(ESP.getFlashChipSize()));
    Serial.printf("Sketch Size       : %u\n", static_cast<unsigned int>(ESP.getSketchSize()));
    Serial.printf("Free Sketch Space : %u\n", static_cast<unsigned int>(ESP.getFreeSketchSpace()));
}

// ---------------------------------------------------
// Build
// ---------------------------------------------------

inline void cliBuild(const String&)
{
    Serial.println();
    Serial.printf("Board   : %s\n", BOARD_NAME);
    Serial.printf("Version : %s\n", FW_DISPLAY_VERSION);
    Serial.printf("Built   : %s %s\n", __DATE__, __TIME__);
}

// ---------------------------------------------------
// Temp
// ---------------------------------------------------

inline void cliTemp(const String& args)
{
#if defined(CONFIG_IDF_TARGET_ESP32C6) || defined(CONFIG_IDF_TARGET_ESP32S3) || defined(CONFIG_IDF_TARGET_ESP32C3)

    String rest = args;
    String sub = cliPopToken(rest);

    float tempC = temperatureRead();

    if (sub.isEmpty())
    {
        Serial.printf("Current: %.2f C\n", tempC);
        Serial.printf("Peak:    %.2f C\n", maxObservedTemp);
        return;
    }

    if (sub.equalsIgnoreCase("reset"))
    {
        maxObservedTemp = temperatureRead();

        Serial.println(
            "Temperature statistics reset"
        );

        return;
    }

    if (sub.equalsIgnoreCase("watch"))
    {
        String option = cliPopToken(rest);

        if (option.isEmpty())
        {
            Serial.printf(
                "Temperature Watch: %s (%ld ms)\n",
                tempWatchEnabled ? "ON" : "OFF",
                t_freq
            );
            return;
        }

        if (option.equalsIgnoreCase("on"))
        {
            tempWatchEnabled = true;
            Serial.printf(
                "Temperature Watch enabled (%ld ms)\n",
                t_freq
            );
            return;
        }

        if (option.equalsIgnoreCase("off"))
        {
            tempWatchEnabled = false;
            Serial.println("Temperature Watch disabled");
            return;
        }

        long seconds = option.toInt();

        if (seconds >= 1 && seconds <= 180)
        {
            t_freq = seconds * 1000;
            tempWatchEnabled = true;

            Serial.printf(
                "Temperature Watch every %ld seconds\n",
                seconds
            );

            return;
        }

        Serial.println("Usage: temp watch [on|off|1-180]");
        return;
    }

    Serial.println("Usage: temp watch [on|off|1-180]");

#else

    Serial.println("Temperature sensor not implemented");

#endif
}

// ---------------------------------------------------
// Reboot
// ---------------------------------------------------

inline void cliReboot(const String&)
{
    Serial.println("Rebooting...");
    Serial.flush();
    delay(200);
    ESP.restart();
}

// ---------------------------------------------------
// RFID (MFRC522)
// ---------------------------------------------------

inline void cliRfid(const String& args)
{
    String rest = args;
    String sub = cliPopToken(rest);

    if (sub.equalsIgnoreCase("ver"))
    {
        byte ver = CLI_RFID_OBJ.PCD_ReadRegister(MFRC522::VersionReg);
        Serial.printf("MFRC522 VersionReg: 0x%02X", ver);
        if (ver == 0x92)
        {
            Serial.println(" (v2.0 / standard clone)");
        }
        else if (ver == 0x91)
        {
            Serial.println(" (v1.0)");
        }
        else if (ver == 0x82)
        {
            Serial.println(" (compatible clone / non-standard version)");
        }
        else if (ver == 0x00 || ver == 0xFF)
        {
            Serial.println(" (SPI comms failure - check MISO/MOSI/SCK/CS/3.3V)");
        }
        else
        {
            Serial.println(" (non-standard/unknown)");
        }
        return;
    }

    if (sub.equalsIgnoreCase("status"))
    {
        byte ver = CLI_RFID_OBJ.PCD_ReadRegister(MFRC522::VersionReg);
        bool spiOk = (ver != 0x00 && ver != 0xFF);
        Serial.printf("RFID SPI Link : %s (VersionReg=0x%02X)\n", spiOk ? "OK" : "FAIL", ver);
        Serial.printf("Last UID len  : %u\n", static_cast<unsigned int>(CLI_RFID_OBJ.uid.size));
        return;
    }

    if (sub.equalsIgnoreCase("reset"))
    {
        Serial.println("Resetting and re-initializing MFRC522...");
        cliDefaultRfidReset();
        byte ver = CLI_RFID_OBJ.PCD_ReadRegister(MFRC522::VersionReg);
        Serial.printf("Post-reset VersionReg: 0x%02X\n", ver);
        return;
    }

    if (sub.equalsIgnoreCase("scan"))
    {
        if (!CLI_RFID_OBJ.PICC_IsNewCardPresent())
        {
            Serial.println("No PICC detected");
            return;
        }
        if (!CLI_RFID_OBJ.PICC_ReadCardSerial())
        {
            Serial.println("Card detected, but failed to read serial/UID");
            return;
        }

        Serial.print("UID:");
        for (byte i = 0; i < CLI_RFID_OBJ.uid.size; i++)
        {
            Serial.printf(" %02X", CLI_RFID_OBJ.uid.uidByte[i]);
        }
        Serial.printf(" (SAK=0x%02X, type=%s)\n",
                      CLI_RFID_OBJ.uid.sak,
                      CLI_RFID_OBJ.PICC_GetTypeName(CLI_RFID_OBJ.PICC_GetType(CLI_RFID_OBJ.uid.sak)));

        CLI_RFID_OBJ.PICC_HaltA();
        CLI_RFID_OBJ.PCD_StopCrypto1();
        return;
    }

    if (sub.equalsIgnoreCase("dump"))
    {
        if (!CLI_RFID_OBJ.PICC_IsNewCardPresent() || !CLI_RFID_OBJ.PICC_ReadCardSerial())
        {
            Serial.println("Place a card/tag on the reader and run 'rfid dump'");
            return;
        }
        CLI_RFID_OBJ.PICC_DumpToSerial(&(CLI_RFID_OBJ.uid));
        CLI_RFID_OBJ.PICC_HaltA();
        CLI_RFID_OBJ.PCD_StopCrypto1();
        return;
    }

    Serial.println("Usage: rfid ver|status|reset|scan|dump");
}

// ---------------------------------------------------
// Display (ST7789 1.54")
// ---------------------------------------------------

inline uint16_t cliParseColor565(const String& name)
{
    if (name.equalsIgnoreCase("red"))    return ST77XX_RED;
    if (name.equalsIgnoreCase("green"))  return ST77XX_GREEN;
    if (name.equalsIgnoreCase("blue"))   return ST77XX_BLUE;
    if (name.equalsIgnoreCase("white"))  return ST77XX_WHITE;
    if (name.equalsIgnoreCase("black"))  return ST77XX_BLACK;
    if (name.equalsIgnoreCase("yellow")) return ST77XX_YELLOW;
    if (name.equalsIgnoreCase("cyan"))   return ST77XX_CYAN;
    if (name.equalsIgnoreCase("magenta"))return ST77XX_MAGENTA;
    return 0;
}

inline void cliDisplay(const String& args)
{
    String rest = args;
    String sub = cliPopToken(rest);

    if (sub.equalsIgnoreCase("status"))
    {
        Serial.printf("Display Size     : %dx%d\n", CLI_TFT_OBJ.width(), CLI_TFT_OBJ.height());
        Serial.printf("Display Rotation : %u\n", static_cast<unsigned int>(CLI_TFT_OBJ.getRotation()));
        return;
    }

    if (sub.equalsIgnoreCase("test"))
    {
        Serial.println("Running ST7789 pattern & text test...");
        CLI_TFT_OBJ.fillScreen(ST77XX_RED);
        delay(200);
        CLI_TFT_OBJ.fillScreen(ST77XX_GREEN);
        delay(200);
        CLI_TFT_OBJ.fillScreen(ST77XX_BLUE);
        delay(200);
        CLI_TFT_OBJ.fillScreen(ST77XX_BLACK);

        int16_t w = CLI_TFT_OBJ.width();
        int16_t h = CLI_TFT_OBJ.height();
        CLI_TFT_OBJ.drawRect(0, 0, w, h, ST77XX_WHITE);
        CLI_TFT_OBJ.drawRect(4, 4, w - 8, h - 8, ST77XX_CYAN);

        CLI_TFT_OBJ.setCursor(14, 20);
        CLI_TFT_OBJ.setTextColor(ST77XX_YELLOW, ST77XX_BLACK);
        CLI_TFT_OBJ.setTextSize(2);
        CLI_TFT_OBJ.println("ST7789 OK");

        CLI_TFT_OBJ.setCursor(14, 48);
        CLI_TFT_OBJ.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
        CLI_TFT_OBJ.setTextSize(1);
        CLI_TFT_OBJ.printf("Board: %s\n", BOARD_NAME);
        CLI_TFT_OBJ.setCursor(14, 62);
        CLI_TFT_OBJ.printf("IP: %s\n", WiFi.localIP().toString().c_str());
        CLI_TFT_OBJ.setCursor(14, 76);
        CLI_TFT_OBJ.printf("Heap: %u\n", static_cast<unsigned int>(ESP.getFreeHeap()));

        Serial.println("Display test complete");

        delay(250);

        cliDisplayRestore();
        return;
    }

    if (sub.equalsIgnoreCase("clear"))
    {
        CLI_TFT_OBJ.fillScreen(ST77XX_BLACK);
        Serial.println("Display cleared");
        cliDisplayRestore();
        return;
    }

    if (sub.equalsIgnoreCase("color"))
    {
        if (rest.isEmpty())
        {
            Serial.println("Usage: display color red|green|blue|white|black|yellow|cyan|magenta");
            return;
        }

        uint16_t c = cliParseColor565(rest);
        if (c == 0 && !rest.equalsIgnoreCase("black"))
        {
            Serial.printf("Unknown color: %s\n", rest.c_str());
            return;
        }

        CLI_TFT_OBJ.fillScreen(c);
        Serial.printf("Display filled with %s\n", rest.c_str());
        cliDisplayRestore();
        return;
    }

    if (sub.equalsIgnoreCase("restore"))
    {
        cliDisplayRestore();
        Serial.printf("Display restored to normal\n");
        return;
    }

    Serial.println("Usage: display status|test|clear|restore|color <name>");
}

// ---------------------------------------------------
// LED
// ---------------------------------------------------

inline void cliLedPinSet(bool on)
{
    if (CLI_LED_PIN < 0 || CLI_LED_PIN == RFID_RST_PIN || CLI_LED_PIN == RFID_SS_PIN || CLI_LED_PIN == RFID_SCK_PIN || CLI_LED_PIN == RFID_MISO_PIN || CLI_LED_PIN == RFID_MOSI_PIN || CLI_LED_PIN == TFT_CS_PIN || CLI_LED_PIN == TFT_DC_PIN || CLI_LED_PIN == TFT_RST_PIN || CLI_LED_PIN == SPK_PIN) return;
    pinMode(CLI_LED_PIN, OUTPUT);
#if CLI_LED_ACTIVE_HIGH
    digitalWrite(CLI_LED_PIN, on ? HIGH : LOW);
#else
    digitalWrite(CLI_LED_PIN, on ? LOW : HIGH);
#endif
}
#if BOARD_HAS_RGB
  #ifndef CLI_RGB_SET_FN
inline void cliRgbSetColor(uint8_t r, uint8_t g, uint8_t b)
{
    static Adafruit_NeoPixel rgbStrip(CLI_RGB_COUNT, CLI_RGB_PIN, NEO_GRB + NEO_KHZ800);
    static bool initialized = false;
    if (!initialized)
    {
        rgbStrip.begin();
        initialized = true;
    }
    for (uint16_t i = 0; i < CLI_RGB_COUNT; i++)
    {
        rgbStrip.setPixelColor(i, rgbStrip.Color(r, g, b));
    }
    rgbStrip.show();
}
  #define CLI_RGB_SET_FN(r, g, b) cliRgbSetColor((r), (g), (b))
#endif
#endif

inline void cliLed(const String& args)
{
    String rest = args;
    String sub = cliPopToken(rest);

#if BOARD_HAS_RGB
    if (sub.equalsIgnoreCase("on"))
    {
        String color = cliPopToken(rest);
        if (color.isEmpty() || color.equalsIgnoreCase("red") || color.equalsIgnoreCase("r"))
        {
            CLI_RGB_SET_FN(255, 0, 0);
            Serial.println("RGB LED ON: Red");
            return;
        }
        if (color.equalsIgnoreCase("green") || color.equalsIgnoreCase("g"))
        {
            CLI_RGB_SET_FN(0, 255, 0);
            Serial.println("RGB LED ON: Green");
            return;
        }
        if (color.equalsIgnoreCase("blue") || color.equalsIgnoreCase("b"))
        {
            CLI_RGB_SET_FN(0, 0, 255);
            Serial.println("RGB LED ON: Blue");
            return;
        }

        Serial.println("Usage: led on [red|green|blue]");
        return;
    }

    if (sub.equalsIgnoreCase("off"))
    {
        CLI_RGB_SET_FN(0, 0, 0);
        Serial.println("RGB LED OFF");
        return;
    }

    if (sub.equalsIgnoreCase("blink"))
    {
        constexpr int kCycles = 5;
        Serial.printf("Cycling RGB LED (Red -> Green -> Blue) for %d iterations...\n", kCycles);
        for (int i = 0; i < kCycles; i++)
        {
            CLI_RGB_SET_FN(255, 0, 0);
            delay(150);
            CLI_RGB_SET_FN(0, 255, 0);
            delay(150);
            CLI_RGB_SET_FN(0, 0, 255);
            delay(150);
        }
        CLI_RGB_SET_FN(0, 0, 0);
        Serial.println("RGB LED blink cycle complete");
        return;
    }

    Serial.println("Usage: led on [red|green|blue] | off | blink");
#else
    if (sub.equalsIgnoreCase("on"))
    {
        cliLedPinSet(true);
        Serial.printf("LED ON (pin %d)\n", CLI_LED_PIN);
        return;
    }

    if (sub.equalsIgnoreCase("off"))
    {
        cliLedPinSet(false);
        Serial.printf("LED OFF (pin %d)\n", CLI_LED_PIN);
        return;
    }

    if (sub.equalsIgnoreCase("blink"))
    {
        int count = 5;
        if (!rest.isEmpty())
        {
            int parsed = rest.toInt();
            if (parsed > 0 && parsed <= 5)
            {
                count = parsed;
            }
        }

        Serial.printf("Blinking LED %d times on pin %d...\n", count, CLI_LED_PIN);
        for (int i = 0; i < count; i++)
        {
            cliLedPinSet(true);
            delay(100);
            cliLedPinSet(false);
            delay(100);
        }
        Serial.println("LED blink complete");
        return;
    }

    Serial.println("Usage: led on|off|blink [count]");
#endif
}

inline void cliFreq(const String& level)
{
    if (level.isEmpty()) { Serial.printf("CPU Frequency: %u MHz\n", getCpuFrequencyMhz()); return; }
    if (level != "80" && level != "160"
#if defined(CONFIG_IDF_TARGET_ESP32S3)
        && level != "240"
#endif
    ) {
        Serial.println("Unsupported CPU frequency for this board");
        return;
    }
    if (!setCpuFrequencyMhz(level.toInt())) {
        Serial.println("CPU frequency change failed");
        return;
    }
    Serial.printf("CPU Frequency: %u MHz\n", getCpuFrequencyMhz());
}

inline void cliAP(const String& args)
{
    String rest = args;
    String sub = cliPopToken(rest);

    if (sub.equalsIgnoreCase("on") || sub.equalsIgnoreCase("off")) {
        Serial.println("Change AP mode through web settings");
        return;
    }

    if (sub.equalsIgnoreCase("status"))
    {
        switch (WiFi.getMode())
        {
            case WIFI_OFF:
                Serial.println("WiFi Mode: OFF");
                break;

            case WIFI_STA:
                Serial.println("WiFi Mode: STA (AP Disabled)");
                break;

            case WIFI_AP:
                Serial.println("WiFi Mode: AP Only");
                break;

            case WIFI_AP_STA:
                Serial.println("WiFi Mode: AP + STA");
                break;

            default:
                Serial.println("WiFi Mode: Unknown");
                break;
        }

        return;
    }
    Serial.println("Usage: ap status");
    return;
}
#endif // DEBUG_CLI