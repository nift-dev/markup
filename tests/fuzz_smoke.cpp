#include <markup/Markup.h>

#include <cstdlib>
#include <iostream>
#include <random>
#include <string>

int main(int argc, char** argv) {
    const int iterations = argc > 1 ? std::atoi(argv[1]) : 10000;
    std::mt19937_64 random(0x4d41524b555050ULL);
    static const std::string alphabet =
        "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789\n\r\t "
        "#*_~`[]()<>!|\\&;:.-+'\"/=";
    for (int n = 0; n < iterations; ++n) {
        std::string input;
        const std::size_t length = static_cast<std::size_t>(random() % 512);
        input.reserve(length);
        for (std::size_t i = 0; i < length; ++i) input.push_back(alphabet[random() % alphabet.size()]);
        markup::Options options;
        options.allow_raw_html = (n % 2) == 0;
        options.standalone = (n % 7) == 0;
        std::string output, error;
        for (const auto format : {markup::Format::Markdown, markup::Format::AsciiDoc,
                                  markup::Format::ReStructuredText}) {
            if (!markup::convert(format, input, output, error, options) || !error.empty()) {
                std::cerr << markup::format_name(format) << " conversion failed at deterministic case "
                          << n << ": " << error << '\n';
                return 1;
            }
        }
    }
    std::cout << iterations << " deterministic mutation cases passed for all formats\n";
}
