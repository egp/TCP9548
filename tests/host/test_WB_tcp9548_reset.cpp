#include "TCP9548.h"
#include "TestSupport.h"
#include "fakes/FakeTCP9548Transport.h"

static void TestWB_TCP9548_Reset_WritesZeroMask() {
  FakeTCP9548Transport transport;
  transport.nextWriteSuccess = true;
  transport.nextReadSuccess = true;
  transport.controlToRead = 0x00;

  TCP9548 mux(transport);

  CHECK(mux.reset());
  CHECK_EQ(transport.writeCallCount, 1);
  CHECK_EQ(transport.readCallCount, 1);
  CHECK_EQ(static_cast<int>(transport.lastWriteAddress), 0x70);
  CHECK_EQ(static_cast<int>(transport.lastWrittenControl), 0x00);
}

static void TestWB_TCP9548_Reset_VerifiesByReadback() {
  FakeTCP9548Transport transport;
  transport.nextWriteSuccess = true;
  transport.nextReadSuccess = true;
  transport.echoWrittenControlOnRead = true;

  TCP9548 mux(transport, 0x77);

  CHECK(mux.reset());
  CHECK_EQ(transport.writeCallCount, 1);
  CHECK_EQ(transport.readCallCount, 1);
  CHECK_EQ(static_cast<int>(transport.lastWriteAddress), 0x77);
  CHECK_EQ(static_cast<int>(transport.lastReadAddress), 0x77);
  CHECK_EQ(static_cast<int>(transport.lastWrittenControl), 0x00);
  CHECK_EQ(static_cast<int>(transport.controlToRead), 0x00);
}

static void TestWB_TCP9548_Reset_FailsWhenWriteFails() {
  FakeTCP9548Transport transport;
  transport.nextWriteSuccess = false;
  transport.nextReadSuccess = true;

  TCP9548 mux(transport);

  CHECK(!mux.reset());
  CHECK_EQ(transport.writeCallCount, 1);
  CHECK_EQ(transport.readCallCount, 0);
}

static void TestWB_TCP9548_Reset_FailsWhenVerifyReadFails() {
  FakeTCP9548Transport transport;
  transport.nextWriteSuccess = true;
  transport.nextReadSuccess = false;

  TCP9548 mux(transport);

  CHECK(!mux.reset());
  CHECK_EQ(transport.writeCallCount, 1);
  CHECK_EQ(transport.readCallCount, 1);
}

static void TestWB_TCP9548_Reset_RejectsInvalidAddressWithoutTransportTraffic() {
  FakeTCP9548Transport transport;
  TCP9548 mux(transport, 0x6F);

  CHECK(!mux.reset());
  CHECK_EQ(transport.writeCallCount, 0);
  CHECK_EQ(transport.readCallCount, 0);
}

int main() {
  int failures = 0;
  RUN_TEST(TestWB_TCP9548_Reset_WritesZeroMask);
  RUN_TEST(TestWB_TCP9548_Reset_VerifiesByReadback);
  RUN_TEST(TestWB_TCP9548_Reset_FailsWhenWriteFails);
  RUN_TEST(TestWB_TCP9548_Reset_FailsWhenVerifyReadFails);
  RUN_TEST(TestWB_TCP9548_Reset_RejectsInvalidAddressWithoutTransportTraffic);
  return failures == 0 ? 0 : 1;
}