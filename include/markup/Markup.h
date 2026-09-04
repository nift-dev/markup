#pragma once

#include <string>

namespace markup {

enum class Format {
    Markdown,
    AsciiDoc,
    ReStructuredText,
};

struct Options {
    // Library callers normally need an embeddable HTML fragment. The CLI's
    // --standalone flag opts into a complete HTML document.
    bool standalone = false;
    bool allow_raw_html = true;
    // Disable Markup++ extensions when measuring CommonMark conformance.
    bool enable_extensions = true;
    std::string title;
};

inline constexpr unsigned api_version = 1;

bool format_for_extension(const std::string& extension, Format& format);
const char* format_name(Format format);
bool is_supported(Format format);

// Converts one UTF-8 string without reading files or using global state.
// On failure, output is cleared and error contains a stable diagnostic.
bool convert(Format format, const std::string& input, std::string& output,
             std::string& error, const Options& options = {});

} // namespace markup
