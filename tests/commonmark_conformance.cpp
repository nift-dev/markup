#include <markup/Markup.h>

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

struct CommonMarkCase {
    int example;
    const char* section;
    const char* markdown;
    const char* html;
};

#include "commonmark_cases.inc"

int main() {
    markup::Options options;
    options.markdown_profile = markup::Options::MarkdownProfile::CommonMark;
    options.allow_raw_html = true;
    options.standalone = false;

    std::vector<int> failures;
    for (const auto& test_case : commonmark_cases) {
        std::string actual;
        std::string error;
        const bool converted = markup::convert(markup::Format::Markdown,
                                               test_case.markdown, actual, error,
                                               options);
        if (!converted || !error.empty() || actual != test_case.html) {
            failures.push_back(test_case.example);
            if (failures.size() <= 10) {
                std::cerr << "CommonMark example " << test_case.example << " ("
                          << test_case.section << ") failed\n";
                if (!error.empty()) std::cerr << "diagnostic: " << error << '\n';
                if (converted && actual != test_case.html) {
                    std::cerr << "expected:\n" << test_case.html
                              << "actual:\n" << actual;
                }
            }
        }
    }

    constexpr std::size_t total = sizeof(commonmark_cases) / sizeof(commonmark_cases[0]);
    std::cout << "CommonMark 0.31.2: " << total - failures.size() << '/' << total
              << " examples passed\n";
    if (!failures.empty()) {
        std::cerr << failures.size() << " CommonMark examples failed\n";
        return 1;
    }
    return 0;
}
