#include <markup/Markup.h>

#include <cstdlib>
#include <iostream>
#include <string>

namespace {
int checks = 0;
markup::Options extended() {
    markup::Options options;
    options.markdown_profile = markup::Options::MarkdownProfile::Extended;
    return options;
}
void expect(const std::string& name, const std::string& markdown,
            const std::string& expected, const markup::Options& options = extended()) {
    std::string html, error;
    if (!markup::convert(markup::Format::Markdown, markdown, html, error, options) || html != expected) {
        std::cerr << "FAIL " << name << "\nexpected:\n" << expected << "actual:\n" << html << error << '\n';
        std::exit(1);
    }
    ++checks;
}
void contains(const std::string& name, const std::string& markdown,
              const std::string& needle, const markup::Options& options = extended()) {
    std::string html, error;
    if (!markup::convert(markup::Format::Markdown, markdown, html, error, options) || html.find(needle) == std::string::npos) {
        std::cerr << "FAIL " << name << ": missing " << needle << "\n" << html << error << '\n';
        std::exit(1);
    }
    ++checks;
}
}

int main() {
    expect("empty", "", "");
    expect("paragraph", "Hello, world!", "<p>Hello, world!</p>\n");
    expect("paragraph escape", "A < B & C > D", "<p>A &lt; B &amp; C &gt; D</p>\n");
    expect("soft break", "one\ntwo", "<p>one\ntwo</p>\n");
    expect("hard break", "one  \ntwo", "<p>one<br>\ntwo</p>\n");
    expect("hard break strips padding", "one   \ntwo", "<p>one<br>\ntwo</p>\n");
    expect("atx h1", "# Heading", "<h1>Heading</h1>\n");
    expect("atx h6", "###### Small", "<h6>Small</h6>\n");
    expect("closing hashes", "## Heading ##", "<h2>Heading</h2>\n");
    expect("setext h1", "Heading\n=======", "<h1>Heading</h1>\n");
    expect("setext h2", "Heading\n-------", "<h2>Heading</h2>\n");
    expect("thematic break", "before\n\n* * *\n\nafter", "<p>before</p>\n<hr>\n<p>after</p>\n");
    expect("emphasis", "*one* and _two_", "<p><em>one</em> and <em>two</em></p>\n");
    expect("strong", "**one** and __two__", "<p><strong>one</strong> and <strong>two</strong></p>\n");
    expect("nested inline", "**bold and *italic***", "<p><strong>bold and <em>italic</em></strong></p>\n");
    expect("strikethrough", "~~gone~~", "<p><del>gone</del></p>\n");
    expect("escaped punctuation", "\\*literal\\*", "<p>*literal*</p>\n");
    expect("inline code", "Use `<tag> & value`.", "<p>Use <code>&lt;tag&gt; &amp; value</code>.</p>\n");
    expect("code span spaces", "`` code ` here ``", "<p><code>code ` here</code></p>\n");
    expect("link", "[Nift](https://nift.dev)", "<p><a href=\"https://nift.dev\">Nift</a></p>\n");
    expect("link title", "[Nift](https://nift.dev \"Site\")", "<p><a href=\"https://nift.dev\" title=\"Site\">Nift</a></p>\n");
    expect("image", "![Logo](logo.png \"Logo\")", "<p><img src=\"logo.png\" alt=\"Logo\" title=\"Logo\"></p>\n");
    expect("autolink", "<https://nift.dev>", "<p><a href=\"https://nift.dev\">https://nift.dev</a></p>\n");
    expect("email autolink", "<hello@example.com>", "<p><a href=\"mailto:hello@example.com\">hello@example.com</a></p>\n");
    expect("entity", "Copyright &copy;", "<p>Copyright &copy;</p>\n");
    expect("unordered list", "- one\n- two", "<ul>\n<li>one</li>\n<li>two</li>\n</ul>\n");
    expect("ordered list", "3. three\n4. four", "<ol start=\"3\">\n<li>three</li>\n<li>four</li>\n</ol>\n");
    expect("task list", "- [ ] open\n- [x] done", "<ul>\n<li><input type=\"checkbox\" disabled> open</li>\n<li><input type=\"checkbox\" disabled checked> done</li>\n</ul>\n");
    expect("blockquote", "> quoted\n> text", "<blockquote>\n<p>quoted\ntext</p>\n</blockquote>\n");
    expect("nested blockquote", "> > deep", "<blockquote>\n<blockquote>\n<p>deep</p>\n</blockquote>\n</blockquote>\n");
    expect("fenced code", "```cpp\nif (a < b) {}\n```", "<pre><code class=\"language-cpp\">if (a &lt; b) {}</code></pre>\n");
    expect("tilde fence", "~~~\na & b\n~~~", "<pre><code>a &amp; b</code></pre>\n");
    expect("unclosed fence", "```\ntext", "<pre><code>text</code></pre>\n");
    expect("indented code", "    alpha\n    beta", "<pre><code>alpha\nbeta</code></pre>\n");
    expect("raw html", "<aside>raw</aside>", "<aside>raw</aside>\n");
    expect("table", "Name | Score\n--- | ---:\nAda | 10\nLinus | 9", "<table>\n<thead>\n<tr><th>Name</th><th style=\"text-align: right\">Score</th></tr>\n</thead>\n<tbody>\n<tr><td>Ada</td><td style=\"text-align: right\">10</td></tr>\n<tr><td>Linus</td><td style=\"text-align: right\">9</td></tr>\n</tbody>\n</table>\n");
    expect("crlf", "# Windows\r\n\r\nText\r\n", "<h1>Windows</h1>\n<p>Text</p>\n");

    markup::Options safe;
    safe.allow_raw_html = false;
    safe.markdown_profile = markup::Options::MarkdownProfile::Extended;
    expect("safe raw html", "<script>alert(1)</script>", "<p>&lt;script&gt;alert(1)&lt;/script&gt;</p>\n", safe);
    expect("safe active link", "[bad](javascript:alert(1))", "<p><a href=\"\">bad</a></p>\n", safe);

    markup::Options strict;
    strict.markdown_profile = markup::Options::MarkdownProfile::CommonMark;
    expect("strict nul replacement", std::string("a\0b", 3), "<p>a\xEF\xBF\xBD" "b</p>\n", strict);
    expect("strict lone carriage return", "one\rtwo", "<p>one\ntwo</p>\n", strict);
    expect("strict tab indentation", "\tcode\n", "<pre><code>code\n</code></pre>\n", strict);
    expect("strict extensions disabled", "~~text~~", "<p>~~text~~</p>\n", strict);

    markup::Options standalone;
    standalone.standalone = true;
    standalone.markdown_profile = markup::Options::MarkdownProfile::Extended;
    standalone.title = "A & B";
    contains("standalone doctype", "# Hello", "<!doctype html>", standalone);
    contains("standalone title", "# Hello", "<title>A &amp; B</title>", standalone);
    contains("standalone body", "# Hello", "<body>\n<h1>Hello</h1>\n</body>", standalone);

    markup::Format format{};
    if (!markup::format_for_extension(".MD", format) || format != markup::Format::Markdown ||
        !markup::format_for_extension("adoc", format) || format != markup::Format::AsciiDoc ||
        !markup::format_for_extension(".rst", format) || format != markup::Format::ReStructuredText ||
        markup::format_for_extension(".txt", format)) return 2;
    ++checks;

    std::string output = "old", error;
    if (markup::convert(markup::Format::AsciiDoc, "= Title", output, error) || !output.empty() ||
        error != "AsciiDoc conversion is not implemented yet") return 3;
    ++checks;
    std::cout << checks << " Markdown smoke checks passed\n";
}
