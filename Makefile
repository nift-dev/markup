CXX ?= g++
CC ?= cc
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra -pedantic
CFLAGS ?= -std=c99 -O2 -Wall -Wextra -pedantic
CPPFLAGS ?= -Iinclude -Ivendor/cmark
LDFLAGS ?=

TARGET := markup
LIBSRC := src/Markup.cpp src/AsciiDoc.cpp
BUILD_DIR := .build
CMARK_DIR := vendor/cmark
CMARK_NAMES := blocks buffer cmark cmark_ctype houdini_href_e houdini_html_e houdini_html_u html inlines iterator node references render scanners utf8
CMARK_SRC := $(addprefix $(CMARK_DIR)/,$(addsuffix .c,$(CMARK_NAMES)))
CMARK_OBJ := $(addprefix $(BUILD_DIR)/cmark/,$(addsuffix .o,$(CMARK_NAMES)))
CMARK_SAN_OBJ := $(addprefix $(BUILD_DIR)/cmark-san/,$(addsuffix .o,$(CMARK_NAMES)))
CMARK_FUZZ_OBJ := $(addprefix $(BUILD_DIR)/cmark-fuzz/,$(addsuffix .o,$(CMARK_NAMES)))
SMOKE := $(BUILD_DIR)/markup-smoke
ADVERSARIAL := $(BUILD_DIR)/markup-adversarial
FUZZ := $(BUILD_DIR)/markup-fuzz
COMMONMARK_REGRESSIONS := $(BUILD_DIR)/commonmark-regressions
PROFILE_MATRIX := $(BUILD_DIR)/profile-matrix
ASCIIDOC := $(BUILD_DIR)/asciidoc
CMARK_REFERENCE := $(BUILD_DIR)/cmark-reference
SANITIZER_FLAGS ?= -O1 -g -fno-omit-frame-pointer -fsanitize=address,undefined
ASAN_OPTIONS ?= detect_leaks=1:halt_on_error=1
UBSAN_OPTIONS ?= halt_on_error=1

all: $(TARGET)

$(TARGET): cli/main.cpp $(LIBSRC) include/markup/Markup.h $(CMARK_OBJ)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) cli/main.cpp $(LIBSRC) $(CMARK_OBJ) $(LDFLAGS) -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/cmark/%.o: $(CMARK_DIR)/%.c
	mkdir -p $(BUILD_DIR)/cmark
	$(CC) -I$(CMARK_DIR) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/cmark-san/%.o: $(CMARK_DIR)/%.c
	mkdir -p $(BUILD_DIR)/cmark-san
	$(CC) -I$(CMARK_DIR) -std=c99 -Wall -Wextra -pedantic $(SANITIZER_FLAGS) -c $< -o $@

$(BUILD_DIR)/cmark-fuzz/%.o: $(CMARK_DIR)/%.c
	mkdir -p $(BUILD_DIR)/cmark-fuzz
	$(CC) -I$(CMARK_DIR) -std=c99 -O1 -g -fno-omit-frame-pointer -fsanitize=fuzzer-no-link,address,undefined -c $< -o $@

$(SMOKE): tests/markdown_smoke.cpp $(LIBSRC) include/markup/Markup.h $(CMARK_OBJ) | $(BUILD_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) tests/markdown_smoke.cpp $(LIBSRC) $(CMARK_OBJ) -o $@

$(ADVERSARIAL): tests/markdown_adversarial.cpp $(LIBSRC) include/markup/Markup.h $(CMARK_OBJ) | $(BUILD_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) tests/markdown_adversarial.cpp $(LIBSRC) $(CMARK_OBJ) -o $@

$(FUZZ): tests/fuzz_smoke.cpp $(LIBSRC) include/markup/Markup.h $(CMARK_OBJ) | $(BUILD_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) tests/fuzz_smoke.cpp $(LIBSRC) $(CMARK_OBJ) -o $@

$(COMMONMARK_REGRESSIONS): tests/commonmark_regressions.cpp $(LIBSRC) include/markup/Markup.h $(CMARK_OBJ) | $(BUILD_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) tests/commonmark_regressions.cpp $(LIBSRC) $(CMARK_OBJ) -o $@

$(PROFILE_MATRIX): tests/profile_matrix.cpp $(LIBSRC) include/markup/Markup.h $(CMARK_OBJ) | $(BUILD_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) tests/profile_matrix.cpp $(LIBSRC) $(CMARK_OBJ) -o $@

$(ASCIIDOC): tests/asciidoc.cpp $(LIBSRC) include/markup/Markup.h src/AsciiDoc.h $(CMARK_OBJ) | $(BUILD_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) tests/asciidoc.cpp $(LIBSRC) $(CMARK_OBJ) -o $@

$(CMARK_REFERENCE): tests/cmark_reference.c $(CMARK_OBJ) | $(BUILD_DIR)
	$(CC) -I$(CMARK_DIR) $(CFLAGS) tests/cmark_reference.c $(CMARK_OBJ) -o $@

test-smoke: $(SMOKE)
	./$(SMOKE)

test-adversarial: $(ADVERSARIAL)
	./$(ADVERSARIAL)

test-cli: $(TARGET)
	MARKUP_BIN="$(CURDIR)/$(TARGET)" bash tests/cli_smoke.sh

test-fuzz: $(FUZZ)
	./$(FUZZ) 10000

test-commonmark-regressions: $(COMMONMARK_REGRESSIONS)
	./$(COMMONMARK_REGRESSIONS)

test-profile-matrix: $(PROFILE_MATRIX)
	./$(PROFILE_MATRIX)

test-asciidoc: $(ASCIIDOC)
	./$(ASCIIDOC)

test-asciidoc-release: $(TARGET) test-asciidoc
	python3 tests/asciidoc_release_gate.py --program ./$(TARGET)

test: test-smoke test-adversarial test-cli test-fuzz test-commonmark-regressions test-profile-matrix test-asciidoc test-asciidoc-release
	python3 tests/asciidoc_fixture_inventory.py

commonmark-report: $(TARGET)
	python3 tests/commonmark_runner.py --program ./$(TARGET) --allow-failures

test-commonmark: $(TARGET)
	python3 tests/commonmark_runner.py --program ./$(TARGET)

test-commonmark-cm2: $(TARGET)
	bash tests/commonmark_sections.sh cm2 ./$(TARGET)

test-commonmark-cm3: $(TARGET)
	bash tests/commonmark_sections.sh cm3 ./$(TARGET)

test-commonmark-cm4: $(TARGET)
	bash tests/commonmark_sections.sh cm4 ./$(TARGET)

test-commonmark-cm5: $(TARGET)
	bash tests/commonmark_sections.sh cm5 ./$(TARGET)

test-commonmark-cm6: $(TARGET)
	bash tests/commonmark_sections.sh cm6 ./$(TARGET)
	python3 tests/commonmark_complexity.py --program ./$(TARGET) --family emphasis

test-commonmark-cm7: $(TARGET)
	bash tests/commonmark_sections.sh cm7 ./$(TARGET)
	python3 tests/commonmark_complexity.py --program ./$(TARGET) --family links

test-commonmark-cm8: test-commonmark test-profile-matrix

test-commonmark-cm9: $(TARGET) $(CMARK_REFERENCE)
	bash tests/commonmark_reproducibility.sh ./$(TARGET)
	python3 tests/commonmark_differential.py --program ./$(TARGET) --reference ./$(CMARK_REFERENCE)

test-performance: $(TARGET)
	python3 tests/commonmark_performance.py --program ./$(TARGET)

fuzz-libfuzzer: $(CMARK_FUZZ_OBJ)
	$(CXX) $(CPPFLAGS) -std=c++17 -O1 -g -fno-omit-frame-pointer -fsanitize=fuzzer,address,undefined tests/fuzz_libfuzzer.cpp $(LIBSRC) $(CMARK_FUZZ_OBJ) -o $(BUILD_DIR)/markup-libfuzzer

test-release-local: test test-commonmark-cm9 test-performance test-asciidoc-release
	python3 tests/commonmark_complexity.py --program ./$(TARGET) --family all

test-sanitize:
	mkdir -p $(BUILD_DIR)
	$(MAKE) $(CMARK_SAN_OBJ)
	$(CXX) $(CPPFLAGS) -std=c++17 -Wall -Wextra -pedantic $(SANITIZER_FLAGS) tests/markdown_smoke.cpp $(LIBSRC) $(CMARK_SAN_OBJ) -o $(BUILD_DIR)/markup-smoke-san
	ASAN_OPTIONS=$(ASAN_OPTIONS) UBSAN_OPTIONS=$(UBSAN_OPTIONS) ./$(BUILD_DIR)/markup-smoke-san
	$(CXX) $(CPPFLAGS) -std=c++17 -Wall -Wextra -pedantic $(SANITIZER_FLAGS) tests/markdown_adversarial.cpp $(LIBSRC) $(CMARK_SAN_OBJ) -o $(BUILD_DIR)/markup-adversarial-san
	ASAN_OPTIONS=$(ASAN_OPTIONS) UBSAN_OPTIONS=$(UBSAN_OPTIONS) ./$(BUILD_DIR)/markup-adversarial-san
	$(CXX) $(CPPFLAGS) -std=c++17 -Wall -Wextra -pedantic $(SANITIZER_FLAGS) tests/commonmark_regressions.cpp $(LIBSRC) $(CMARK_SAN_OBJ) -o $(BUILD_DIR)/commonmark-regressions-san
	ASAN_OPTIONS=$(ASAN_OPTIONS) UBSAN_OPTIONS=$(UBSAN_OPTIONS) ./$(BUILD_DIR)/commonmark-regressions-san
	$(CXX) $(CPPFLAGS) -std=c++17 -Wall -Wextra -pedantic $(SANITIZER_FLAGS) tests/profile_matrix.cpp $(LIBSRC) $(CMARK_SAN_OBJ) -o $(BUILD_DIR)/profile-matrix-san
	ASAN_OPTIONS=$(ASAN_OPTIONS) UBSAN_OPTIONS=$(UBSAN_OPTIONS) ./$(BUILD_DIR)/profile-matrix-san
	$(CXX) $(CPPFLAGS) -std=c++17 -Wall -Wextra -pedantic $(SANITIZER_FLAGS) tests/asciidoc.cpp $(LIBSRC) $(CMARK_SAN_OBJ) -o $(BUILD_DIR)/asciidoc-san
	ASAN_OPTIONS=$(ASAN_OPTIONS) UBSAN_OPTIONS=$(UBSAN_OPTIONS) ./$(BUILD_DIR)/asciidoc-san

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

.PHONY: all test test-smoke test-adversarial test-cli test-fuzz test-commonmark-regressions test-profile-matrix test-asciidoc test-asciidoc-release test-sanitize commonmark-report test-commonmark test-commonmark-cm2 test-commonmark-cm3 test-commonmark-cm4 test-commonmark-cm5 test-commonmark-cm6 test-commonmark-cm7 test-commonmark-cm8 test-commonmark-cm9 test-performance fuzz-libfuzzer test-release-local clean
