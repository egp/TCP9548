#include <Arduino.h>
#include <TCP3231.h>
#include <TCP9548.h>

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#ifndef TCP9548_SMOKE_ADDRESS
#define TCP9548_SMOKE_ADDRESS 0x70
#endif

#ifndef TCP9548_SMOKE_SDA_PIN
#define TCP9548_SMOKE_SDA_PIN SDA
#endif

#ifndef TCP9548_SMOKE_SCL_PIN
#define TCP9548_SMOKE_SCL_PIN SCL
#endif

#ifndef TCP9548_SMOKE_CLOCK_HZ
#define TCP9548_SMOKE_CLOCK_HZ 100000UL
#endif

static BBI2C gBus;
static TCP9548 gMux(gBus, TCP9548_SMOKE_ADDRESS);

static char gLine[48];
static size_t gLineLen = 0;

static void printHexByte(uint8_t value) {
  if (value < 0x10) {
    Serial.print('0');
  }
  Serial.print(value, HEX);
}

static void printMask(uint8_t control) {
  Serial.print(F("control=0x"));
  printHexByte(control);
  Serial.print(F(" bits="));
  for (int8_t bit = 7; bit >= 0; --bit) {
    Serial.print(((control >> bit) & 0x01u) ? '1' : '0');
  }
  Serial.println();
}

static void printLastError(const __FlashStringHelper* op) {
  Serial.print(op);
  Serial.print(F(": "));
  Serial.println(gMux.errorString());
}

static void printHelp() {
  Serial.println(F("Commands:"));
  Serial.println(F("  ?            help"));
  Serial.println(F("  b            begin/probe"));
  Serial.println(F("  r            read control"));
  Serial.println(F("  w <mask>     write control (decimal or 0xHH)"));
  Serial.println(F("  s <ch>       select channel 0..7"));
  Serial.println(F("  e <ch>       enable channel 0..7"));
  Serial.println(F("  d <ch>       disable channel 0..7"));
  Serial.println(F("  c <ch>       read channel enabled"));
  Serial.println(F("  x            reset (write 0x00 and verify)"));
}

static void printPrompt() {
  Serial.print(F("> "));
}

static bool parseUint8Arg(const char* text, uint8_t& value) {
  char* end = nullptr;
  unsigned long parsed = 0;

  while (*text == ' ' || *text == '\t') {
    ++text;
  }
  if (*text == '\0') {
    return false;
  }

  parsed = strtoul(text, &end, 0);

  while (*end == ' ' || *end == '\t') {
    ++end;
  }
  if (*end != '\0') {
    return false;
  }
  if (parsed > 255UL) {
    return false;
  }

  value = static_cast<uint8_t>(parsed);
  return true;
}

static void readAndPrintControl() {
  uint8_t control = 0;
  if (!gMux.readControl(control)) {
    printLastError(F("readControl"));
    return;
  }
  printMask(control);
}

static void handleCommand(char* line) {
  char cmd = '\0';
  uint8_t value = 0;
  bool enabled = false;

  while (*line == ' ' || *line == '\t') {
    ++line;
  }
  if (*line == '\0') {
    return;
  }

  cmd = static_cast<char>(tolower(static_cast<unsigned char>(*line)));
  ++line;
  while (*line == ' ' || *line == '\t') {
    ++line;
  }

  switch (cmd) {
    case '?':
    case 'h':
      printHelp();
      return;

    case 'b':
      if (!gMux.begin()) {
        printLastError(F("begin"));
        return;
      }
      Serial.println(F("begin: ok"));
      return;

    case 'r':
      readAndPrintControl();
      return;

    case 'w':
      if (!parseUint8Arg(line, value)) {
        Serial.println(F("usage: w <mask>"));
        return;
      }
      if (!gMux.writeControl(value)) {
        printLastError(F("writeControl"));
        return;
      }
      Serial.println(F("writeControl: ok"));
      readAndPrintControl();
      return;

    case 's':
      if (!parseUint8Arg(line, value)) {
        Serial.println(F("usage: s <channel>"));
        return;
      }
      if (!gMux.selectChannel(value)) {
        printLastError(F("selectChannel"));
        return;
      }
      Serial.println(F("selectChannel: ok"));
      readAndPrintControl();
      return;

    case 'e':
      if (!parseUint8Arg(line, value)) {
        Serial.println(F("usage: e <channel>"));
        return;
      }
      if (!gMux.enableChannel(value)) {
        printLastError(F("enableChannel"));
        return;
      }
      Serial.println(F("enableChannel: ok"));
      readAndPrintControl();
      return;

    case 'd':
      if (!parseUint8Arg(line, value)) {
        Serial.println(F("usage: d <channel>"));
        return;
      }
      if (!gMux.disableChannel(value)) {
        printLastError(F("disableChannel"));
        return;
      }
      Serial.println(F("disableChannel: ok"));
      readAndPrintControl();
      return;

    case 'c':
      if (!parseUint8Arg(line, value)) {
        Serial.println(F("usage: c <channel>"));
        return;
      }
      if (!gMux.readChannelEnabled(value, enabled)) {
        printLastError(F("readChannelEnabled"));
        return;
      }
      Serial.print(F("channel "));
      Serial.print(value);
      Serial.print(F(": "));
      Serial.println(enabled ? F("enabled") : F("disabled"));
      return;

    case 'x':
      if (!gMux.reset()) {
        printLastError(F("reset"));
        return;
      }
      Serial.println(F("reset: ok"));
      readAndPrintControl();
      return;

    default:
      Serial.println(F("unknown command"));
      printHelp();
      return;
  }
}

void setup() {
  memset(&gBus, 0, sizeof(gBus));
  gBus.bWire = 0;
  gBus.iSDA = TCP9548_SMOKE_SDA_PIN;
  gBus.iSCL = TCP9548_SMOKE_SCL_PIN;
  I2CInit(&gBus, TCP9548_SMOKE_CLOCK_HZ);

  Serial.begin(115200);
  while (!Serial && millis() < 3000UL) {
  }

  Serial.println();
  Serial.println(F("TCP9548 HardwareSmoke"));
  Serial.print(F("address=0x"));
  printHexByte(TCP9548_SMOKE_ADDRESS);
  Serial.println();
  Serial.print(F("SDA pin="));
  Serial.println(TCP9548_SMOKE_SDA_PIN);
  Serial.print(F("SCL pin="));
  Serial.println(TCP9548_SMOKE_SCL_PIN);
  Serial.print(F("clock="));
  Serial.println(TCP9548_SMOKE_CLOCK_HZ);

  printHelp();
  if (gMux.begin()) {
    Serial.println(F("initial probe: ok"));
    readAndPrintControl();
  } else {
    printLastError(F("begin"));
  }
  printPrompt();
}

void loop() {
  while (Serial.available() > 0) {
    const char ch = static_cast<char>(Serial.read());

    if (ch == '\r') {
      continue;
    }
    if (ch == '\n') {
      gLine[gLineLen] = '\0';
      if (gLineLen > 0) {
        handleCommand(gLine);
      }
      gLineLen = 0;
      printPrompt();
      continue;
    }
    if (gLineLen + 1 < sizeof(gLine)) {
      gLine[gLineLen++] = ch;
    }
  }
}