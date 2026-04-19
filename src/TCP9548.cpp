#include "TCP9548.h"
#include "internal/TCP9548Transport.h"

TCP9548::TCP9548(BBI2C& bus, uint8_t address)
    : bus_(&bus), transport_(nullptr), address_(address), lastError_("ok") {}

#if defined(TCP9548_HOST_TEST)
TCP9548::TCP9548(TCP9548Transport& transport, uint8_t address)
    : bus_(nullptr), transport_(&transport), address_(address), lastError_("ok") {}
#endif

bool TCP9548::begin() {
  uint8_t control = 0;
  if (!isValidAddress(address_)) {
    setError("invalid address");
    return false;
  }
  if (!readControlRaw(control)) {
    return false;
  }
  setError("ok");
  return true;
}

bool TCP9548::readControl(uint8_t& control) {
  if (!isValidAddress(address_)) {
    control = 0xFF;
    setError("invalid address");
    return false;
  }
  if (!readControlRaw(control)) {
    return false;
  }
  setError("ok");
  return true;
}

bool TCP9548::writeControl(uint8_t control) {
  if (!isValidAddress(address_)) {
    setError("invalid address");
    return false;
  }
  return writeControlVerified(control);
}

bool TCP9548::selectChannel(uint8_t channel) {
  if (!isValidChannel(channel)) {
    setError("invalid channel");
    return false;
  }
  return writeControl(channelBit(channel));
}

bool TCP9548::enableChannel(uint8_t channel) {
  uint8_t control = 0;
  const uint8_t bit = channelBit(channel);

  if (!isValidChannel(channel)) {
    setError("invalid channel");
    return false;
  }
  if (!readControl(control)) {
    return false;
  }
  if ((control & bit) != 0) {
    setError("ok");
    return true;
  }
  control = static_cast<uint8_t>(control | bit);
  return writeControlVerified(control);
}

bool TCP9548::disableChannel(uint8_t channel) {
  uint8_t control = 0;
  const uint8_t bit = channelBit(channel);

  if (!isValidChannel(channel)) {
    setError("invalid channel");
    return false;
  }
  if (!readControl(control)) {
    return false;
  }
  if ((control & bit) == 0) {
    setError("ok");
    return true;
  }
  control = static_cast<uint8_t>(control & static_cast<uint8_t>(~bit));
  return writeControlVerified(control);
}

bool TCP9548::readChannelEnabled(uint8_t channel, bool& enabled) {
  uint8_t control = 0;

  enabled = false;
  if (!isValidChannel(channel)) {
    setError("invalid channel");
    return false;
  }
  if (!readControl(control)) {
    return false;
  }
  enabled = (control & channelBit(channel)) != 0;
  setError("ok");
  return true;
}

bool TCP9548::reset() {
  if (!isValidAddress(address_)) {
    setError("invalid address");
    return false;
  }
  return writeControlVerified(0x00);
}

const char* TCP9548::errorString() const {
  return lastError_;
}

bool TCP9548::readControlRaw(uint8_t& control) {
#if defined(TCP9548_HOST_TEST)
  (void)bus_;
  if (transport_ == nullptr) {
    control = 0xFF;
    setError("transport not configured");
    return false;
  }
  if (!transport_->readControl(address_, control)) {
    setError("read failed");
    return false;
  }
  return true;
#else
  (void)bus_;
  control = 0xFF;
  setError("not implemented");
  return false;
#endif
}

bool TCP9548::writeControlRaw(uint8_t control) {
#if defined(TCP9548_HOST_TEST)
  (void)bus_;
  if (transport_ == nullptr) {
    (void)control;
    setError("transport not configured");
    return false;
  }
  if (!transport_->writeControl(address_, control)) {
    setError("write failed");
    return false;
  }
  return true;
#else
  (void)bus_;
  (void)control;
  setError("not implemented");
  return false;
#endif
}

bool TCP9548::writeControlVerified(uint8_t control) {
  uint8_t verify = 0xFF;
  if (!writeControlRaw(control)) {
    return false;
  }
  if (!readControlRaw(verify)) {
    return false;
  }
  if (verify != control) {
    setError("verify mismatch");
    return false;
  }
  setError("ok");
  return true;
}

bool TCP9548::isValidAddress(uint8_t address) {
  return address >= 0x70 && address <= 0x77;
}

bool TCP9548::isValidChannel(uint8_t channel) {
  return channel <= 7;
}

uint8_t TCP9548::channelBit(uint8_t channel) {
  return static_cast<uint8_t>(1u << channel);
}

void TCP9548::setError(const char* message) {
  lastError_ = message;
}