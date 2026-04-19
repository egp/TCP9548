CXX ?= c++
CXXFLAGS := -std=c++17 -Wall -Wextra -Werror -pedantic -DTCP9548_HOST_TEST -I./src -I./tests/host

HOST_TEST_SOURCES := $(wildcard tests/host/test_*.cpp)
HOST_TEST_BINS := $(patsubst tests/host/%.cpp,build/%,$(HOST_TEST_SOURCES))

.PHONY: all test clean

all: test

test: $(HOST_TEST_BINS)
	@set -e; \
	for t in $(HOST_TEST_BINS); do \
	  echo "==> $$t"; \
	  $$t; \
	done

build/%: tests/host/%.cpp src/TCP9548.cpp src/TCP9548.h src/internal/TCP9548Transport.h tests/host/TestSupport.h tests/host/fakes/FakeTCP9548Transport.h
	@mkdir -p $(@D)
	@$(CXX) $(CXXFLAGS) -o $@ $< src/TCP9548.cpp || { echo "compile failed: $<"; exit 1; }

clean:
	@rm -rf build