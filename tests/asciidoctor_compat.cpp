#include <markup/Markup.h>
#include <cstdlib>
#include <iostream>
#include <string>

namespace {
int checks = 0;
void contains(const std::string& source, const std::string& fragment) {
    std::string html, error;
    if (!markup::convert(markup::Format::AsciiDoc, source, html, error) ||
        html.find(fragment) == std::string::npos) {
        std::cerr << "Asciidoctor compatibility mismatch: " << error << "\n" << html;
        std::exit(1);
    }
    ++checks;
}
}

int main() {
    contains("A paragraph.\n", "<div class=\"paragraph\">\n<p>A paragraph.</p>");
    contains("= Guide\nAda Example\n1.0, 2026-09-05\n\n== Start\n\nHello.\n", "<h2>Start</h2>");
    contains(".Example\n====\ninside\n====\n", "<div class=\"exampleblock\">");
    contains("[source,cpp]\n----\nint main() {}\n----\n", "<div class=\"source\">");
    contains("____\nquoted\n____\n", "<blockquote>");
    contains("****\nsidebar\n****\n", "<div class=\"sidebarblock\">");
    contains("////\nhidden\n////\n\nshown\n", "<p>shown</p>");
    std::cout << checks << " Asciidoctor core compatibility checks passed\n";
}
