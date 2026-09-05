#include <markup/Markup.h>

#include <cstdlib>
#include <iostream>
#include <string>

namespace {
int checks = 0;

void expect(const std::string& source, const std::string& expected) {
    std::string output;
    std::string error;
    if (!markup::convert(markup::Format::AsciiDoc, source, output, error) ||
        !error.empty() || output != expected) {
        std::cerr << "AsciiDoc mismatch\nsource:\n" << source << "expected:\n"
                  << expected << "actual:\n" << output << "error: " << error << '\n';
        std::exit(1);
    }
    ++checks;
}
} // namespace

int main() {
    expect("", "");
    expect("body only\n", "<div class=\"paragraph\">\n<p>body only</p>\n</div>\n");
    expect("first\r\nline\rsecond", "<div class=\"paragraph\">\n<p>first\nline\nsecond</p>\n</div>\n");
    expect("caf\xc3\xa9", "<div class=\"paragraph\">\n<p>caf\xc3\xa9</p>\n</div>\n");
    expect(std::string("a\0b", 3), "<div class=\"paragraph\">\n<p>a\xef\xbf\xbd" "b</p>\n</div>\n");
    expect("<tag> & text", "<div class=\"paragraph\">\n<p>&lt;tag&gt; &amp; text</p>\n</div>\n");

    std::string output, error;
    if (!markup::is_supported(markup::Format::AsciiDoc) ||
        !markup::convert(markup::Format::AsciiDoc, "repeat", output, error)) return 2;
    const std::string first = output;
    if (!markup::convert(markup::Format::AsciiDoc, "repeat", output, error) || output != first) return 3;
    checks += 2;
    std::cout << checks << " AsciiDoc model/invariant checks passed\n";
}
