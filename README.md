# TCP9548

Arduino library for the TCA9548/TCA9548A 8-channel I2C switch using BitBang_I2C via the TCP3231-style `BBI2C` transport pattern instead of `Wire`.

## Status

This repo is in early TDD setup.

Current contents:
- requirements spec
- initial repo scaffolding
- host-test harness
- first red-cycle WB tests
- intentionally incorrect production stubs

## Design goals

- Use caller-owned `BBI2C`
- No `Wire`
- `bool` return values plus `errorString()`
- Read/write the control register
- Verify every write by reading the control register back
- Keep almost all behavior host-testable
- Reserve hardware-specific code for the lowest transport layer

## Target boards

- Arduino Uno R3
- Arduino Uno R4 Minima
- Arduino Uno R4 WiFi

## Planned API

```cpp
class TCP9548 {
public:
  explicit TCP9548(BBI2C& bus, uint8_t address = 0x70);

  bool begin();

  bool readControl(uint8_t& control);
  bool writeControl(uint8_t control);

  bool selectChannel(uint8_t channel);
  bool enableChannel(uint8_t channel);
  bool disableChannel(uint8_t channel);
  bool disableAllChannels();

  bool readChannelEnabled(uint8_t channel, bool& enabled);

  // Logical reset to default state: write 0x00 and verify by readback.
  bool reset();

  const char* errorString() const;
};