#include "TCP9548.h"
#include "TestSupport.h"
#include "fakes/FakeTCP9548Transport.h"

static void TestWB_TCP9548_Begin_UsesDefaultAddress0x70() {
  FakeTCP9548Transport transport;
  transport.nextReadSuccess = true;
  transport.controlToRead = 0x00;

  TCP9548 mux(transport);

  CHECK(mux.begin());
  CHECK_EQ(transport.readCallCount, 1);
  CHECK_EQ(transport.writeCallCount, 0);
  CHECK_EQ(static_cast<int>(transport.lastReadAddress), 0x70);
}

static void TestWB_TCP9548_Begin_AcceptsEachValidAddress() {
  for (uint8_t address = 0x70; address <= 0x77; ++address) {
    FakeTCP9548Transport transport;
    transport.nextReadSuccess = true;
    transport.controlToRead = 0x00;

    TCP9548 mux(transport, address);

    CHECK(mux.begin());
    CHECK_EQ(transport.readCallCount, 1);
    CHECK_EQ(transport.writeCallCount, 0);
    CHECK_EQ(static_cast<int>(transport.lastReadAddress), static_cast<int>(address));
  }
}

static void TestWB_TCP9548_Begin_RejectsAddressBelowRange() {
  FakeTCP9548Transport transport;
  TCP9548 mux(transport, 0x6F);

  CHECK(!mux.begin());
  CHECK_EQ(transport.readCallCount, 0);
  CHECK_EQ(transport.writeCallCount, 0);
}

static void TestWB_TCP9548_Begin_RejectsAddressAboveRange() {
  FakeTCP9548Transport transport;
  TCP9548 mux(transport, 0x78);

  CHECK(!mux.begin());
  CHECK_EQ(transport.readCallCount, 0);
  CHECK_EQ(transport.writeCallCount, 0);
}

static void TestWB_TCP9548_Begin_PropagatesReadFailure() {
  FakeTCP9548Transport transport;
  transport.nextReadSuccess = false;

  TCP9548 mux(transport);

  CHECK(!mux.begin());
  CHECK_EQ(transport.readCallCount, 1);
  CHECK_EQ(transport.writeCallCount, 0);
}

static void TestWB_TCP9548_Begin_ProbeIsReadOnly() {
  FakeTCP9548Transport transport;
  transport.nextReadSuccess = true;
  transport.controlToRead = 0xA5;

  TCP9548 mux(transport);

  CHECK(mux.begin());
  CHECK_EQ(transport.readCallCount, 1);
  CHECK_EQ(transport.writeCallCount, 0);
  CHECK_EQ(static_cast<int>(transport.controlToRead), 0xA5);
}

int main() {
  int failures = 0;
  RUN_TEST(TestWB_TCP9548_Begin_UsesDefaultAddress0x70);
  RUN_TEST(TestWB_TCP9548_Begin_AcceptsEachValidAddress);
  RUN_TEST(TestWB_TCP9548_Begin_RejectsAddressBelowRange);
  RUN_TEST(TestWB_TCP9548_Begin_RejectsAddressAboveRange);
  RUN_TEST(TestWB_TCP9548_Begin_PropagatesReadFailure);
  RUN_TEST(TestWB_TCP9548_Begin_ProbeIsReadOnly);
  return failures == 0 ? 0 : 1;
}