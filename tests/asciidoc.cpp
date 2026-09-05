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
    expect("= Document Title\n\nbody\n",
           "<div id=\"header\">\n<h1>Document Title</h1>\n</div>\n"
           "<div class=\"paragraph\">\n<p>body</p>\n</div>\n");
    expect("= {project} Guide\nAda Example <ada@example.test>\nv1.2, 2026-09-05\n"
           ":project: Markup++\n:unused: gone\n:unused!:\n\nWelcome to {project}.\n",
           "<div id=\"header\">\n<h1>Markup++ Guide</h1>\n"
           "<div class=\"details\">Ada Example &lt;ada@example.test&gt;</div>\n</div>\n"
           "<div class=\"paragraph\">\n<p>Welcome to Markup++.</p>\n</div>\n");
    expect("== Parent\n\nparent body\n\n=== Child {name}\n\nchild body\n",
           "<div class=\"sect1\">\n<h2>Parent</h2>\n"
           "<div class=\"paragraph\">\n<p>parent body</p>\n</div>\n"
           "<div class=\"sect2\">\n<h3>Child {name}</h3>\n"
           "<div class=\"paragraph\">\n<p>child body</p>\n</div>\n</div>\n</div>\n");
    expect(".Program\n[source,cpp]\n----\nint main() {\n  return 0;\n}\n----\n",
           "<div class=\"source\">\n<div class=\"title\">Program</div>\n"
           "<pre>int main() {\n  return 0;\n}</pre>\n</div>\n");
    expect("****\nsidebar text\n****\n",
           "<div class=\"sidebarblock\">\n<div class=\"paragraph\">\n"
           "<p>sidebar text</p>\n</div>\n</div>\n");
    expect("////\nnot rendered\n////\n\n'''\n<<<\n",
           "<hr>\n<div class=\"pagebreak\"></div>\n");
    expect("----\nunclosed & literal\n",
           "<div class=\"listingblock\">\n<pre>unclosed &amp; literal</pre>\n</div>\n");

    std::string output, error;
    if (!markup::is_supported(markup::Format::AsciiDoc) ||
        !markup::convert(markup::Format::AsciiDoc, "repeat", output, error)) return 2;
    const std::string first = output;
    if (!markup::convert(markup::Format::AsciiDoc, "repeat", output, error) || output != first) return 3;
    checks += 2;
    std::cout << checks << " AsciiDoc model/invariant checks passed\n";
}
