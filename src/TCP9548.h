#ifndef TCP9548_H
#define TCP9548_H

#if defined(ARDUINO)
#include <Arduino.h>
#include <BitBang_I2C.h>
#else
#include <stdint.h>
class BBI2C;
#endif

class TCP9548Transport;

class TCP9548 {
public:
  explicit TCP9548(BBI2C& bus, uint8_t address = 0x70);

#if defined(TCP9548_HOST_TEST)
  explicit TCP9548(TCP9548Transport& transport, uint8_t address = 0x70);
#endif

  bool begin();

  bool readControl(uint8_t& control);
  bool writeControl(uint8_t control);

  bool selectChannel(uint8_t channel);
  bool enableChannel(uint8_t channel);
  bool disableChannel(uint8_t channel);
  bool disableAllChannels();

  bool readChannelEnabled(uint8_t channel, bool& enabled);

  bool reset();

  const char* errorString() const;

private:
  bool readControlRaw(uint8_t& control);
  bool writeControlRaw(uint8_t control);
  bool writeControlVerified(uint8_t control);

  static bool isValidAddress(uint8_t address);
  static bool isValidChannel(uint8_t channel);
  static uint8_t channelBit(uint8_t channel);

  void setError(const char* message);

  BBI2C* bus_;
  TCP9548Transport* transport_;
  uint8_t address_;
  const char* lastError_;
};

#endif