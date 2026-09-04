#include <markup/Markup.h>

#include <cstddef>
#include <cstdint>
#include <string>

extern "C" int LLVMFuzzerTestOneInput(const std::uint8_t* data, std::size_t size) {
    const std::string input(reinterpret_cast<const char*>(data), size);
    for (const bool raw : {false, true}) {
        markup::Options options;
        options.allow_raw_html = raw;
        std::string output, error;
        if (!markup::convert(markup::Format::Markdown, input, output, error, options)) {
            __builtin_trap();
        }
    }
    return 0;
}
