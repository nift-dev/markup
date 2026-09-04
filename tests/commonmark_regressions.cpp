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

    // CM4 - container blocks.
    expect("mixed nested containers", "> - one\n>   - two\n",
           "<blockquote>\n<ul>\n<li>one\n<ul>\n<li>two</li>\n</ul>\n</li>\n</ul>\n</blockquote>\n");
    expect("loose list", "- one\n\n- two\n",
           "<ul>\n<li>\n<p>one</p>\n</li>\n<li>\n<p>two</p>\n</li>\n</ul>\n");
    expect("multi-block item", "- one\n\n  continued\n",
           "<ul>\n<li>\n<p>one</p>\n<p>continued</p>\n</li>\n</ul>\n");
    expect("lazy quote continuation", "> quote\nlazy\n",
           "<blockquote>\n<p>quote\nlazy</p>\n</blockquote>\n");
    expect("bullet change starts list", "- a\n+ b\n",
           "<ul>\n<li>a</li>\n</ul>\n<ul>\n<li>b</li>\n</ul>\n");
    expect("empty list item", "*\n* b\n", "<ul>\n<li></li>\n<li>b</li>\n</ul>\n");

    // CM5 - inline lexical primitives.
    expect("punctuation escapes only", "\\*x\\* \\a\n", "<p>*x* \\a</p>\n");
    expect("entities decode and invalid escapes", "&copy; &#x41; &madeup;\n",
           "<p>© A &amp;madeup;</p>\n");
    expect("arbitrary code delimiter", "`` foo ` bar ``\n",
           "<p><code>foo ` bar</code></p>\n");
    expect("backslash hard break", "one\\\ntwo\n", "<p>one<br />\ntwo</p>\n");
    expect("email autolink", "<foo+bar@example.com>\n",
           "<p><a href=\"mailto:foo+bar@example.com\">foo+bar@example.com</a></p>\n");
    expect("invalid URI autolink", "<http://foo.bar/baz bim>\n",
           "<p>&lt;http://foo.bar/baz bim&gt;</p>\n");
    expect("invalid UTF-8 replacement", std::string("bad \xFF byte\n", 11),
           "<p>bad \xEF\xBF\xBD byte</p>\n");

    // CM6 - emphasis delimiter behavior.
    expect("intraword underscore", "foo_bar_\n", "<p>foo_bar_</p>\n");
    expect("punctuation flanking", "a**\"foo\"**\n",
           "<p>a**&quot;foo&quot;**</p>\n");
    expect("nested emphasis", "*foo **bar** baz*\n",
           "<p><em>foo <strong>bar</strong> baz</em></p>\n");
    expect("escaped delimiter content", "foo **\\***\n",
           "<p>foo <strong>*</strong></p>\n");
    expect("autolink delimiter precedence", "__a<https://foo.bar/?q=__>\n",
           "<p>__a<a href=\"https://foo.bar/?q=__\">https://foo.bar/?q=__</a></p>\n");

    // CM7 - links, images and references.
    expect("full reference", "[foo]: /url \"title\"\n\n[foo]\n",
           "<p><a href=\"/url\" title=\"title\">foo</a></p>\n");
    expect("definition inside quote", "[foo]\n\n> [foo]: /url\n",
           "<p><a href=\"/url\">foo</a></p>\n<blockquote>\n</blockquote>\n");
    expect("inline destination escaping", "[link](foo\\bar)\n",
           "<p><a href=\"foo%5Cbar\">link</a></p>\n");
    expect("no links inside links", "[foo [bar](/uri)][ref]\n\n[ref]: /uri\n",
           "<p>[foo <a href=\"/uri\">bar</a>]<a href=\"/uri\">ref</a></p>\n");
    expect("reference precedence", "[foo][bar][baz]\n\n[baz]: /url1\n[foo]: /url2\n",
           "<p>[foo]<a href=\"/url1\">bar</a></p>\n");
    expect("image title", "![foo](/url \"title\")\n",
           "<p><img src=\"/url\" alt=\"foo\" title=\"title\" /></p>\n");
    expect("escaped image opener", "\\![foo]\n\n[foo]: /url \"title\"\n",
           "<p>!<a href=\"/url\" title=\"title\">foo</a></p>\n");

    std::cout << checks << " CommonMark checkpoint regressions passed\n";
}
