#ifndef FAKE_TCP9548_TRANSPORT_H
#define FAKE_TCP9548_TRANSPORT_H

#include <stdint.h>

#include "internal/TCP9548Transport.h"

class FakeTCP9548Transport : public TCP9548Transport {
public:
  bool nextReadSuccess = true;
  bool nextWriteSuccess = true;
  bool echoWrittenControlOnRead = true;

  uint8_t controlToRead = 0x00;

  int readCallCount = 0;
  int writeCallCount = 0;

  uint8_t lastReadAddress = 0xFF;
  uint8_t lastWriteAddress = 0xFF;
  uint8_t lastWrittenControl = 0xFF;

  bool readControl(uint8_t address, uint8_t& control) override {
    ++readCallCount;
    lastReadAddress = address;
    control = controlToRead;
    return nextReadSuccess;
  }

  bool writeControl(uint8_t address, uint8_t control) override {
    ++writeCallCount;
    lastWriteAddress = address;
    lastWrittenControl = control;
    if (!nextWriteSuccess) {
      return false;
    }
    if (echoWrittenControlOnRead) {
      controlToRead = control;
    }
    return true;
  }
};

#endif