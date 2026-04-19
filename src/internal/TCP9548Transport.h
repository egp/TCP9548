#ifndef TCP9548_TRANSPORT_H
#define TCP9548_TRANSPORT_H

#include <stdint.h>

class TCP9548Transport {
public:
  virtual ~TCP9548Transport() {}
  virtual bool readControl(uint8_t address, uint8_t& control) = 0;
  virtual bool writeControl(uint8_t address, uint8_t control) = 0;
};

#endif