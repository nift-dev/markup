#include <markup/Markup.h>

#include <cstdlib>
#include <iostream>
#include <string>

namespace {
int checks = 0;

void expect(const std::string& name, const std::string& markdown,
            const std::string& expected, bool allow_raw_html = true) {
    markup::Options options;
    options.enable_extensions = false;
    options.allow_raw_html = allow_raw_html;
    std::string html, error;
    if (!markup::convert(markup::Format::Markdown, markdown, html, error, options) ||
        html != expected) {
        std::cerr << "FAIL " << name << "\nexpected:\n" << expected
                  << "actual:\n" << html << error << '\n';
        std::exit(1);
    }
    ++checks;
}
}

int main() {
    // CM2 - leaf blocks and precedence.
    expect("thematic break beats list", "* * *\n", "<hr />\n");
    expect("setext beats thematic break", "Foo\n---\n", "<h2>Foo</h2>\n");
    expect("longer closing fence", "``` foo\nbar\n````\n",
           "<pre><code class=\"language-foo\">bar\n</code></pre>\n");
    expect("unclosed fence", "~~~\na & b\n", "<pre><code>a &amp; b\n</code></pre>\n");
    expect("heading interrupts paragraph", "foo\n# bar\n",
           "<p>foo</p>\n<h1>bar</h1>\n");
    expect("indented continuation is paragraph", "foo\n    bar\n",
           "<p>foo\nbar</p>\n");

    // CM3 - HTML blocks and inline HTML.
    expect("HTML comment block", "<!-- open\nstill -->\n", "<!-- open\nstill -->\n");
    expect("CDATA block", "<![CDATA[x < y]]>\n", "<![CDATA[x < y]]>\n");
    expect("inline tags", "before <i>x</i> after\n",
           "<p>before <i>x</i> after</p>\n");
    expect("angle bracket text", "2 < 3 and 4 > 1\n",
           "<p>2 &lt; 3 and 4 &gt; 1</p>\n");
    expect("strict safe block omission", "<script>bad()</script>\n",
           "<!-- raw HTML omitted -->\n", false);
    expect("strict safe inline omission", "before <i>x</i> after\n",
           "<p>before <!-- raw HTML omitted -->x<!-- raw HTML omitted --> after</p>\n",
           false);

    std::cout << checks << " CommonMark checkpoint regressions passed\n";
}
