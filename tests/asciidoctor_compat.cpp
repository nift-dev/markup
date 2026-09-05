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
    contains("*strong* _emphasis_ `code` #mark# ^up^ ~down~\n", "<strong>strong</strong> <em>emphasis</em> <code>code</code>");
    contains("word**strong**word and *outer _inner_*\n", "word<strong>strong</strong>word and <strong>outer <em>inner</em></strong>");
    contains("\\*escaped* (C) (R) (TM) left -> right\n", "*escaped* © ® ™ left → right");
    contains("`<tag> & value`\n", "<code>&lt;tag&gt; &amp; value</code>");
    std::cout << checks << " Asciidoctor core compatibility checks passed\n";
}
