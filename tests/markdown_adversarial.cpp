#include <markup/Markup.h>

#include <iostream>
#include <string>
#include <vector>

int main() {
    const std::vector<std::string> cases = {
        "*", "**", "***", "[", "]", "[x]", "[x](", "![x](", "`", "``", "```",
        "<", ">", "<&", "&", "&#;", "&#x;", "&unterminated", "\\", "\\*",
        "#nospace", "####### too many", "-", "--", "1.", "999999999999. item",
        "|", "| --- |", "a | b\n---", ">", "> >", "~~~lang\nunterminated",
        std::string("nul\0byte", 8), std::string(4096, '*'), std::string(4096, '`'),
        std::string(8192, '<'), std::string(8192, '&')
    };
    for (const bool safe : {false, true}) {
        markup::Options options;
        options.allow_raw_html = !safe;
        for (std::size_t i = 0; i < cases.size(); ++i) {
            std::string output = "sentinel", error = "sentinel";
            if (!markup::convert(markup::Format::Markdown, cases[i], output, error, options) || !error.empty()) {
                std::cerr << "unexpected failure for adversarial case " << i << ": " << error << '\n';
                return 1;
            }
        }
    }
    markup::Options safe;
    safe.allow_raw_html = false;
    std::string output, error;
    if (!markup::convert(markup::Format::Markdown,
            "<img src=x onerror=alert(1)> [x](JaVaScRiPt:alert(1))", output, error, safe) ||
        output.find("<img") != std::string::npos || output.find("javascript:") != std::string::npos ||
        output.find("JaVaScRiPt:") != std::string::npos) {
        std::cerr << "safe mode active-content rejection failed: " << output << '\n';
        return 1;
    }
    std::cout << cases.size() * 2 + 1 << " adversarial checks passed\n";
}
