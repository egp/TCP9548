# TCP9548 Requirements Specification
# Library: TCP9548
# Target platform: Arduino
# Transport: BitBang_I2C via caller-owned BBI2C

## 1. Purpose

### 1.1 Objective
TCP9548 shall provide a small, predictable Arduino library for controlling a TCA9548/TCA9548A 8-channel I2C switch using BitBang_I2C, not Wire.

### 1.2 Design intent
TCP9548 shall be a thin mux-control library, not a general I2C framework.

Its responsibilities shall be:
1. select or deselect downstream mux channels,
2. report the mux control state,
3. optionally perform a hardware reset if a reset pin is wired,
4. fit naturally beside libraries such as TCP3231 that already use a caller-owned BBI2C.

## 2. Scope

### 2.1 In scope
TCP9548 shall support:
1. construction against a caller-owned BBI2C bus object,
2. communication with one TCA9548/TCA9548A device at a configured address,
3. reading the current 8-bit channel mask,
4. writing the current 8-bit channel mask,
5. enabling or disabling individual channels,
6. selecting exactly one channel,
7. disabling all channels,
8. optional hardware reset through an Arduino GPIO pin,
9. human-readable error reporting.

### 2.2 Out of scope
TCP9548 shall not:
1. own or initialize the BBI2C bus object,
2. proxy arbitrary downstream device transactions,
3. automatically detect attached downstream devices,
4. manage pull-ups, bus capacitance, or electrical design issues,
5. use interrupts, RTOS features, dynamic allocation, or exceptions.

## 3. External constraints

### 3.1 Device model
The target device shall be treated as an I2C-controlled 8-channel switch with a single 8-bit control register where each bit controls one channel. Multiple channels may be enabled simultaneously.

### 3.2 Addressing
The library shall support the 7-bit address range:
- 0x70 through 0x77

This range corresponds to hardware address pins A2:A0.

### 3.3 Reset/default behavior
Power-on reset and hardware reset both place the device into the default state with all channels deselected.

### 3.4 Bus-speed ceiling
TCP9548 shall not claim support beyond 400 kHz device operation.

### 3.5 Multi-channel caution
Because the device allows multiple channels to be enabled simultaneously, the library shall expose that capability, but the documentation shall warn that multi-channel use may require additional electrical care, including capacitance considerations.

## 4. Functional requirements

### 4.1 Transport and ownership

#### 4.1.1
TCP9548 shall use BitBang_I2C transport through a caller-owned BBI2C instance.

#### 4.1.2
TCP9548 shall not expose Wire in its public API.

#### 4.1.3
TCP9548 shall store a reference to the caller-owned BBI2C instance rather than copying or allocating a bus object.

#### 4.1.4
TCP9548 shall follow the same broad API style as TCP3231:
- constructor injection of BBI2C&
- bool return for operations that may fail
- errorString() for the most recent error

### 4.2 Construction and initialization

#### 4.2.1
TCP9548 shall provide a constructor accepting:
1. BBI2C& bus
2. optional device address, defaulting to 0x70

#### 4.2.2
TCP9548 may provide an overload or constructor option for an optional reset GPIO pin.

#### 4.2.3
begin() shall verify communication with the mux by reading the control register.

#### 4.2.4
begin() shall not modify the mux channel state.

### 4.3 Control register access

#### 4.3.1
TCP9548 shall provide an operation to read the current 8-bit channel mask from the device.

#### 4.3.2
TCP9548 shall provide an operation to write the full 8-bit channel mask to the device.

#### 4.3.3
The library shall treat bit n as the enable state for channel n, for channels 0..7.

#### 4.3.4
The library shall allow any valid 8-bit combination, including:
- no channels enabled,
- exactly one channel enabled,
- multiple channels enabled.

#### 4.3.5
The library shall not invent software-only channel state; the device register shall be the source of truth.

### 4.4 Convenience operations

#### 4.4.1
TCP9548 shall provide disableAllChannels().

#### 4.4.2
TCP9548 shall provide selectChannel(channel) that enables exactly one channel and disables all others.

#### 4.4.3
TCP9548 shall provide enableChannel(channel) that sets the corresponding bit while leaving other bits unchanged.

#### 4.4.4
TCP9548 shall provide disableChannel(channel) that clears the corresponding bit while leaving other bits unchanged.

#### 4.4.5
TCP9548 shall provide isChannelEnabled(channel, enabled) or equivalent functionality derivable from readChannelMask().

#### 4.4.6
Channel arguments outside 0..7 shall be rejected as invalid input.

### 4.5 Transaction semantics

#### 4.5.1
When updating channel selection, TCP9548 shall perform a complete write transaction ending with a STOP condition, because the selected channel becomes active only after a stop condition.

#### 4.5.2
TCP9548 shall not assume a channel is active before the write transaction has completed successfully.

#### 4.5.3
The implementation shall write exactly one control byte per logical register update.

### 4.6 Reset support

#### 4.6.1
If a reset GPIO pin is configured, TCP9548 shall provide hardwareReset().

#### 4.6.2
hardwareReset() shall drive the reset pin active low for at least the datasheet minimum and then release it.

#### 4.6.3
After a successful hardware reset, TCP9548 shall treat the expected device state as 0x00 until proven otherwise by a subsequent read.

#### 4.6.4
If no reset GPIO pin was configured, hardwareReset() shall fail cleanly with a descriptive error.

### 4.7 Error handling

#### 4.7.1
Operations that can fail shall return bool.

#### 4.7.2
TCP9548 shall provide const char* errorString() const.

#### 4.7.3
The library shall report at least these error classes:
1. invalid device address,
2. invalid channel index,
3. invalid read buffer or write buffer, if applicable,
4. I2C register read failure,
5. I2C register write failure,
6. reset pin not configured,
7. begin/probe failure.

#### 4.7.4
On success, the library shall set the last error string to "ok" or equivalent stable success text.

### 4.8 Resource and portability constraints

#### 4.8.1
TCP9548 shall avoid dynamic memory allocation.

#### 4.8.2
TCP9548 shall avoid exceptions.

#### 4.8.3
TCP9548 shall be suitable for small Arduino targets, including AVR-class boards.

#### 4.8.4
TCP9548 shall use fixed-width integer types for public data values where appropriate.

## 5. Documentation requirements

### 5.1
The library shall include a README with:
1. purpose,
2. wiring assumptions,
3. address selection,
4. single-channel example,
5. multi-channel warning,
6. reset-pin behavior.

### 5.2
The README shall clearly state that downstream device libraries continue using the same upstream BBI2C bus, and TCP9548 only changes which downstream branch is connected.

### 5.3
The README shall recommend selectChannel() as the default safe pattern when the user is switching among identical devices that share the same downstream slave address.

## 6. Example requirements

### 6.1
The library shall include a minimal example that:
1. creates a BBI2C bus,
2. creates a TCP9548,
3. calls begin(),
4. selects one channel,
5. reads back and prints the channel mask.

### 6.2
The library should include a second example demonstrating repeated one-hot switching among two or more channels.

### 6.3
If reset support is implemented in phase 1, an example should demonstrate optional reset-pin use.

## 7. Test requirements

### 7.1
The library shall have unit-testable logic for:
1. address validation,
2. channel validation,
3. bit-mask manipulation,
4. one-hot channel selection,
5. multi-channel enable/disable behavior,
6. post-reset expected state handling.

### 7.2
The library shall have hardware-smoke coverage for:
1. begin/probe,
2. read current mask,
3. write mask,
4. select each channel 0 through 7,
5. disable all channels,
6. optional hardware reset.

### 7.3
Acceptance tests shall verify:
1. power-up read reports 0x00 on a freshly reset device,
2. selectChannel(n) yields mask 1 << n,
3. enableChannel(a) then enableChannel(b) yields both bits set,
4. disableChannel(n) clears only bit n,
5. disableAllChannels() yields 0x00.

## 8. Proposed minimal phase-1 API

class TCP9548 {
public:
  explicit TCP9548(BBI2C& bus, uint8_t address = 0x70);
  TCP9548(BBI2C& bus, uint8_t address, int8_t resetPin);

  bool begin();

  bool readChannelMask(uint8_t& mask);
  bool writeChannelMask(uint8_t mask);

  bool selectChannel(uint8_t channel);
  bool enableChannel(uint8_t channel);
  bool disableChannel(uint8_t channel);
  bool disableAllChannels();

  bool hardwareReset();

  const char* errorString() const;
};

## 9. Recommended phase-1 scope

For phase 1, TCP9548 should remain deliberately small:
1. caller-owned BBI2C,
2. default address 0x70,
3. begin(),
4. readChannelMask(),
5. writeChannelMask(),
6. selectChannel(),
7. enableChannel(),
8. disableChannel(),
9. disableAllChannels(),
10. errorString(),
11. optional reset-pin support only if already needed.

That is enough for a clean first slice without overcommitting the API.