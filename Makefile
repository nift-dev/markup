CXX ?= g++
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra -pedantic
CPPFLAGS ?= -Iinclude
LDFLAGS ?=

TARGET := markup
LIBSRC := src/Markup.cpp
BUILD_DIR := .build
SMOKE := $(BUILD_DIR)/markup-smoke
ADVERSARIAL := $(BUILD_DIR)/markup-adversarial
FUZZ := $(BUILD_DIR)/markup-fuzz
SANITIZER_FLAGS ?= -O1 -g -fno-omit-frame-pointer -fsanitize=address,undefined
ASAN_OPTIONS ?= detect_leaks=1:halt_on_error=1
UBSAN_OPTIONS ?= halt_on_error=1

all: $(TARGET)

$(TARGET): cli/main.cpp $(LIBSRC) include/markup/Markup.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) cli/main.cpp $(LIBSRC) $(LDFLAGS) -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(SMOKE): tests/markdown_smoke.cpp $(LIBSRC) include/markup/Markup.h | $(BUILD_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) tests/markdown_smoke.cpp $(LIBSRC) -o $@

$(ADVERSARIAL): tests/markdown_adversarial.cpp $(LIBSRC) include/markup/Markup.h | $(BUILD_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) tests/markdown_adversarial.cpp $(LIBSRC) -o $@

$(FUZZ): tests/fuzz_smoke.cpp $(LIBSRC) include/markup/Markup.h | $(BUILD_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) tests/fuzz_smoke.cpp $(LIBSRC) -o $@

test-smoke: $(SMOKE)
	./$(SMOKE)

test-adversarial: $(ADVERSARIAL)
	./$(ADVERSARIAL)

test-cli: $(TARGET)
	MARKUP_BIN="$(CURDIR)/$(TARGET)" bash tests/cli_smoke.sh

test-fuzz: $(FUZZ)
	./$(FUZZ) 10000

test: test-smoke test-adversarial test-cli test-fuzz

commonmark-report: $(TARGET)
	python3 tests/commonmark_runner.py --program ./$(TARGET) --allow-failures

test-commonmark: $(TARGET)
	python3 tests/commonmark_runner.py --program ./$(TARGET)

test-sanitize:
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CPPFLAGS) -std=c++17 -Wall -Wextra -pedantic $(SANITIZER_FLAGS) tests/markdown_smoke.cpp $(LIBSRC) -o $(BUILD_DIR)/markup-smoke-san
	ASAN_OPTIONS=$(ASAN_OPTIONS) UBSAN_OPTIONS=$(UBSAN_OPTIONS) ./$(BUILD_DIR)/markup-smoke-san
	$(CXX) $(CPPFLAGS) -std=c++17 -Wall -Wextra -pedantic $(SANITIZER_FLAGS) tests/markdown_adversarial.cpp $(LIBSRC) -o $(BUILD_DIR)/markup-adversarial-san
	ASAN_OPTIONS=$(ASAN_OPTIONS) UBSAN_OPTIONS=$(UBSAN_OPTIONS) ./$(BUILD_DIR)/markup-adversarial-san

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

.PHONY: all test test-smoke test-adversarial test-cli test-fuzz test-sanitize commonmark-report test-commonmark clean
