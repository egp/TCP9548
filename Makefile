CXX ?= c++
CXXFLAGS := -std=c++17 -Wall -Wextra -Werror -pedantic -DTCP9548_HOST_TEST -I./src -I./tests/host

ARDUINO_CLI ?= arduino-cli
ARDUINO_WARNINGS ?= all
ARDUINO_SKETCHBOOK ?= $(HOME)/Documents/Arduino
TCP3231_DIR ?= $(ARDUINO_SKETCHBOOK)/libraries/TCP3231

SKETCH := examples/HardwareSmoke

FQBN_UNO_R3 := arduino:avr:uno
FQBN_UNO_R4_MINIMA := arduino:renesas_uno:minima
FQBN_UNO_R4_WIFI := arduino:renesas_uno:unor4wifi

BUILD_DIR := build
HOST_BUILD_DIR := $(BUILD_DIR)
ARDUINO_BUILD_DIR := $(BUILD_DIR)/arduino

HOST_TEST_SOURCES := $(wildcard tests/host/test_*.cpp)
HOST_TEST_BINS := $(patsubst tests/host/%.cpp,$(HOST_BUILD_DIR)/%,$(HOST_TEST_SOURCES))

.PHONY: \
	all \
	test \
	clean \
	arduino-update-index \
	arduino-install-cores \
	arduino-install \
	arduino-check-tcp3231 \
	smoke-uno-r3 \
	smoke-uno-r4-minima \
	smoke-uno-r4-wifi \
	smoke-all

all: test

test: $(HOST_TEST_BINS)
	@set -e; \
	for t in $(HOST_TEST_BINS); do \
	  echo "==> $$t"; \
	  $$t; \
	done

$(HOST_BUILD_DIR)/%: tests/host/%.cpp src/TCP9548.cpp src/TCP9548.h src/internal/TCP9548Transport.h tests/host/TestSupport.h tests/host/fakes/FakeTCP9548Transport.h
	@mkdir -p $(@D)
	@$(CXX) $(CXXFLAGS) -o $@ $< src/TCP9548.cpp || { echo "compile failed: $<"; exit 1; }

arduino-update-index:
	$(ARDUINO_CLI) core update-index

arduino-install-cores: arduino-update-index
	$(ARDUINO_CLI) core install arduino:avr
	$(ARDUINO_CLI) core install arduino:renesas_uno

arduino-install: arduino-install-cores

arduino-check-tcp3231:
	@test -f "$(TCP3231_DIR)/src/TCP3231.h" || { \
		echo "TCP3231 not found at $(TCP3231_DIR)"; \
		echo "Install or symlink TCP3231 there, or override TCP3231_DIR=..."; \
		exit 1; \
	}

smoke-uno-r3: arduino-check-tcp3231
	@mkdir -p $(ARDUINO_BUILD_DIR)/uno-r3
	$(ARDUINO_CLI) compile \
		--fqbn "$(FQBN_UNO_R3)" \
		--warnings "$(ARDUINO_WARNINGS)" \
		--libraries . \
		"$(SKETCH)"

smoke-uno-r4-minima: arduino-check-tcp3231
	@mkdir -p $(ARDUINO_BUILD_DIR)/uno-r4-minima
	$(ARDUINO_CLI) compile \
		--fqbn "$(FQBN_UNO_R4_MINIMA)" \
		--warnings "$(ARDUINO_WARNINGS)" \
		--libraries . \
		"$(SKETCH)"

smoke-uno-r4-wifi: arduino-check-tcp3231
	@mkdir -p $(ARDUINO_BUILD_DIR)/uno-r4-wifi
	$(ARDUINO_CLI) compile \
		--fqbn "$(FQBN_UNO_R4_WIFI)" \
		--warnings "$(ARDUINO_WARNINGS)" \
		--libraries . \
		"$(SKETCH)"

smoke-all: smoke-uno-r3 smoke-uno-r4-minima smoke-uno-r4-wifi

clean:
	@rm -rf $(BUILD_DIR)