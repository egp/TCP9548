#include "TCP9548.h"
#include "TestSupport.h"
#include "fakes/FakeTCP9548Transport.h"

static void TestWB_TCP9548_ReadControl_ReturnsRawValue() {
  FakeTCP9548Transport transport;
  uint8_t control = 0x00;

  transport.controlToRead = 0xA5;

  TCP9548 mux(transport);

  CHECK(mux.readControl(control));
  CHECK_EQ(static_cast<int>(control), 0xA5);
  CHECK_EQ(transport.readCallCount, 1);
  CHECK_EQ(transport.writeCallCount, 0);
}

static void TestWB_TCP9548_WriteControl_WritesThenVerifiesByReadback() {
  FakeTCP9548Transport transport;

  transport.nextWriteSuccess = true;
  transport.nextReadSuccess = true;
  transport.echoWrittenControlOnRead = true;

  TCP9548 mux(transport);

  CHECK(mux.writeControl(0x5A));
  CHECK_EQ(transport.writeCallCount, 1);
  CHECK_EQ(transport.readCallCount, 1);
  CHECK_EQ(static_cast<int>(transport.lastWrittenControl), 0x5A);
  CHECK_EQ(static_cast<int>(transport.controlToRead), 0x5A);
}

static void TestWB_TCP9548_WriteControl_FailsOnVerifyMismatch() {
  FakeTCP9548Transport transport;

  transport.nextWriteSuccess = true;
  transport.nextReadSuccess = true;
  transport.echoWrittenControlOnRead = false;
  transport.controlToRead = 0x00;

  TCP9548 mux(transport);

  CHECK(!mux.writeControl(0x5A));
  CHECK_EQ(transport.writeCallCount, 1);
  CHECK_EQ(transport.readCallCount, 1);
  CHECK_EQ(static_cast<int>(transport.lastWrittenControl), 0x5A);
  CHECK_EQ(static_cast<int>(transport.controlToRead), 0x00);
}

static void TestWB_TCP9548_ReadChannelEnabled_ReturnsTrueWhenBitSet() {
  FakeTCP9548Transport transport;
  bool enabled = false;

  transport.controlToRead = 0x20;

  TCP9548 mux(transport);

  CHECK(mux.readChannelEnabled(5, enabled));
  CHECK(enabled);
  CHECK_EQ(transport.readCallCount, 1);
  CHECK_EQ(transport.writeCallCount, 0);
}

static void TestWB_TCP9548_ReadChannelEnabled_ReturnsFalseWhenBitClear() {
  FakeTCP9548Transport transport;
  bool enabled = true;

  transport.controlToRead = 0x20;

  TCP9548 mux(transport);

  CHECK(mux.readChannelEnabled(4, enabled));
  CHECK(!enabled);
  CHECK_EQ(transport.readCallCount, 1);
  CHECK_EQ(transport.writeCallCount, 0);
}

static void TestWB_TCP9548_ReadChannelEnabled_RejectsInvalidChannelWithoutTraffic() {
  FakeTCP9548Transport transport;
  bool enabled = true;

  TCP9548 mux(transport);

  CHECK(!mux.readChannelEnabled(8, enabled));
  CHECK(!enabled);
  CHECK_EQ(transport.readCallCount, 0);
  CHECK_EQ(transport.writeCallCount, 0);
}

static void TestWB_TCP9548_SelectChannel_WritesOneHotMask() {
  FakeTCP9548Transport transport;

  transport.nextWriteSuccess = true;
  transport.nextReadSuccess = true;
  transport.echoWrittenControlOnRead = true;

  TCP9548 mux(transport);

  CHECK(mux.selectChannel(3));
  CHECK_EQ(transport.writeCallCount, 1);
  CHECK_EQ(transport.readCallCount, 1);
  CHECK_EQ(static_cast<int>(transport.lastWrittenControl), 0x08);
  CHECK_EQ(static_cast<int>(transport.controlToRead), 0x08);
}

static void TestWB_TCP9548_SelectChannel_RejectsInvalidChannelWithoutTraffic() {
  FakeTCP9548Transport transport;

  TCP9548 mux(transport);

  CHECK(!mux.selectChannel(8));
  CHECK_EQ(transport.readCallCount, 0);
  CHECK_EQ(transport.writeCallCount, 0);
}

static void TestWB_TCP9548_EnableChannel_WhenAlreadyEnabledIsReadOnly() {
  FakeTCP9548Transport transport;

  transport.controlToRead = 0x0A;

  TCP9548 mux(transport);

  CHECK(mux.enableChannel(1));
  CHECK_EQ(transport.readCallCount, 1);
  CHECK_EQ(transport.writeCallCount, 0);
  CHECK_EQ(static_cast<int>(transport.controlToRead), 0x0A);
}

static void TestWB_TCP9548_EnableChannel_WhenDisabledOrsInBitAndVerifies() {
  FakeTCP9548Transport transport;

  transport.nextWriteSuccess = true;
  transport.nextReadSuccess = true;
  transport.echoWrittenControlOnRead = true;
  transport.controlToRead = 0x08;

  TCP9548 mux(transport);

  CHECK(mux.enableChannel(1));
  CHECK_EQ(transport.readCallCount, 2);
  CHECK_EQ(transport.writeCallCount, 1);
  CHECK_EQ(static_cast<int>(transport.lastWrittenControl), 0x0A);
  CHECK_EQ(static_cast<int>(transport.controlToRead), 0x0A);
}

static void TestWB_TCP9548_EnableChannel_RejectsInvalidChannelWithoutTraffic() {
  FakeTCP9548Transport transport;

  TCP9548 mux(transport);

  CHECK(!mux.enableChannel(8));
  CHECK_EQ(transport.readCallCount, 0);
  CHECK_EQ(transport.writeCallCount, 0);
}

static void TestWB_TCP9548_DisableChannel_WhenAlreadyDisabledIsReadOnly() {
  FakeTCP9548Transport transport;

  transport.controlToRead = 0x08;

  TCP9548 mux(transport);

  CHECK(mux.disableChannel(1));
  CHECK_EQ(transport.readCallCount, 1);
  CHECK_EQ(transport.writeCallCount, 0);
  CHECK_EQ(static_cast<int>(transport.controlToRead), 0x08);
}

static void TestWB_TCP9548_DisableChannel_WhenEnabledClearsBitAndVerifies() {
  FakeTCP9548Transport transport;

  transport.nextWriteSuccess = true;
  transport.nextReadSuccess = true;
  transport.echoWrittenControlOnRead = true;
  transport.controlToRead = 0x0A;

  TCP9548 mux(transport);

  CHECK(mux.disableChannel(1));
  CHECK_EQ(transport.readCallCount, 2);
  CHECK_EQ(transport.writeCallCount, 1);
  CHECK_EQ(static_cast<int>(transport.lastWrittenControl), 0x08);
  CHECK_EQ(static_cast<int>(transport.controlToRead), 0x08);
}

static void TestWB_TCP9548_DisableChannel_RejectsInvalidChannelWithoutTraffic() {
  FakeTCP9548Transport transport;

  TCP9548 mux(transport);

  CHECK(!mux.disableChannel(8));
  CHECK_EQ(transport.readCallCount, 0);
  CHECK_EQ(transport.writeCallCount, 0);
}

int main() {
  int failures = 0;
  RUN_TEST(TestWB_TCP9548_ReadControl_ReturnsRawValue);
  RUN_TEST(TestWB_TCP9548_WriteControl_WritesThenVerifiesByReadback);
  RUN_TEST(TestWB_TCP9548_WriteControl_FailsOnVerifyMismatch);
  RUN_TEST(TestWB_TCP9548_ReadChannelEnabled_ReturnsTrueWhenBitSet);
  RUN_TEST(TestWB_TCP9548_ReadChannelEnabled_ReturnsFalseWhenBitClear);
  RUN_TEST(TestWB_TCP9548_ReadChannelEnabled_RejectsInvalidChannelWithoutTraffic);
  RUN_TEST(TestWB_TCP9548_SelectChannel_WritesOneHotMask);
  RUN_TEST(TestWB_TCP9548_SelectChannel_RejectsInvalidChannelWithoutTraffic);
  RUN_TEST(TestWB_TCP9548_EnableChannel_WhenAlreadyEnabledIsReadOnly);
  RUN_TEST(TestWB_TCP9548_EnableChannel_WhenDisabledOrsInBitAndVerifies);
  RUN_TEST(TestWB_TCP9548_EnableChannel_RejectsInvalidChannelWithoutTraffic);
  RUN_TEST(TestWB_TCP9548_DisableChannel_WhenAlreadyDisabledIsReadOnly);
  RUN_TEST(TestWB_TCP9548_DisableChannel_WhenEnabledClearsBitAndVerifies);
  RUN_TEST(TestWB_TCP9548_DisableChannel_RejectsInvalidChannelWithoutTraffic);
  return failures == 0 ? 0 : 1;
}