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
    contains("* one\n** nested\n* two\n", "<ul>\n<li>one\n<ul>");
    contains("2. two\n3. three\n", "<ol start=\"2\">");
    contains("Term:: meaning\n", "<dl>\n<dt>Term</dt>");
    contains("|===\n|A |B\n|1 |2\n|===\n", "<table>\n<tr>");
    contains("[[start]]\n== Start\n\nSee <<start,begin>>.\n", "href=\"#start\">begin</a>");
    contains("link:https://example.test[site] image:diagram.png[diagram]\n", "<img src=\"diagram.png\" alt=\"diagram\">");
    contains("= {product} Guide\n:product: Markup++\n:enabled:\n\nifdef::enabled[]\n{product} enabled.\nendif::[]\n", "<p>Markup++ enabled.</p>");
    contains("= Unset\n:value: before\n:value!:\n\n{value}\n", "<p>{value}</p>");
    std::cout << checks << " Asciidoctor core compatibility checks passed\n";
}
