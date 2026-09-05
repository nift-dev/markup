#include <markup/Markup.h>
#include <cstdlib>
#include <iostream>
#include <string>

int main() {
    std::string output, error;
    std::string mixed;
    for (int i = 0; i < 2000; ++i) {
        mixed += "Section " + std::to_string(i) + "\n---------\n\n";
        mixed += "* item **strong** [1]_\n\n";
        mixed += "=====  =====\nA      B\n=====  =====\n1      2\n=====  =====\n\n";
    }
    if (!markup::convert(markup::Format::ReStructuredText, mixed, output, error) ||
        output.empty() || !error.empty()) return 1;

    const std::string hostile(250000, '*');
    if (!markup::convert(markup::Format::ReStructuredText, hostile, output, error) ||
        output.empty()) return 2;

    const std::string malformed = "+---+\n| cell\n+\n.. unknown:: x\n\n:bad:`value`\n";
    for (int i = 0; i < 100; ++i)
        if (!markup::convert(markup::Format::ReStructuredText, malformed, output, error)) return 3;
    std::cout << "reStructuredText robustness corpus passed\n";
}
