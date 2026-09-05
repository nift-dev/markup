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
    expect("* water\n* earth\n** stone\n* air\n",
           "<ul>\n<li>water</li>\n<li>earth\n<ul>\n<li>stone</li>\n</ul>\n</li>\n"
           "<li>air</li>\n</ul>\n");
    expect("3. third\n4. fourth\n",
           "<ol start=\"3\">\n<li>third</li>\n<li>fourth</li>\n</ol>\n");
    expect("* [ ] pending\n* [x] complete\n",
           "<ul class=\"checklist\">\n<li><input type=\"checkbox\" disabled> pending</li>\n"
           "<li><input type=\"checkbox\" disabled checked> complete</li>\n</ul>\n");
    expect("Term:: definition\nOther:: another\n",
           "<dl>\n<dt>Term</dt>\n<dd>definition</dd>\n"
           "<dt>Other</dt>\n<dd>another</dd>\n</dl>\n");
    expect("* principal\n+\ncontinued block\n* next\n",
           "<ul>\n<li>principal\n<div class=\"paragraph\">\n<p>continued block</p>\n</div>\n"
           "</li>\n<li>next</li>\n</ul>\n");
    expect("*strong* _emphasis_ `code & text` #mark# ^up^ ~down~\n",
           "<div class=\"paragraph\">\n<p><strong>strong</strong> <em>emphasis</em> "
           "<code>code &amp; text</code> <mark>mark</mark> <sup>up</sup> <sub>down</sub></p>\n</div>\n");
    expect("word*literal*word word**strong**word and *outer _inner_*\n",
           "<div class=\"paragraph\">\n<p>word*literal*word word<strong>strong</strong>word "
           "and <strong>outer <em>inner</em></strong></p>\n</div>\n");
    expect("\\*escaped* (C) (R) (TM) left -> right +\nnext\n",
           "<div class=\"paragraph\">\n<p>*escaped* \xc2\xa9 \xc2\xae \xe2\x84\xa2 left \xe2\x86\x92 right"
           "<br>\nnext</p>\n</div>\n");
    expect("----\n*not strong* (C)\n----\n",
           "<div class=\"listingblock\">\n<pre>*not strong* (C)</pre>\n</div>\n");
    expect("== *Strong* section\n", "<div class=\"sect1\">\n<h2><strong>Strong</strong> section</h2>\n</div>\n");
    expect("9999999999. not an ordered marker\n",
           "<div class=\"paragraph\">\n<p>9999999999. not an ordered marker</p>\n</div>\n");
    expect("[[intro]]\n== Introduction\n\nSee <<intro,Start here>>, "
           "link:https://example.test[the site], mailto:team@example.test[mail] and "
           "image:diagram.png[System diagram].\n",
           "<div class=\"sect1\" id=\"intro\">\n<h2>Introduction</h2>\n"
           "<div class=\"paragraph\">\n<p>See <a href=\"#intro\">Start here</a>, "
           "<a href=\"https://example.test\">the site</a>, "
           "<a href=\"mailto:team@example.test\">mail</a> and "
           "<img src=\"diagram.png\" alt=\"System diagram\">.</p>\n</div>\n</div>\n");
    expect("Visit https://example.test/path now.\n",
           "<div class=\"paragraph\">\n<p>Visit <a href=\"https://example.test/path\">"
           "https://example.test/path</a> now.</p>\n</div>\n");
    expect(".Inventory\n|===\n|Name |Count\n|Bolts |4\n|2+|Total\n|===\n",
           "<table>\n<caption>Inventory</caption>\n<tr>\n<td>Name</td>\n<td>Count</td>\n</tr>\n"
           "<tr>\n<td>Bolts</td>\n<td>4</td>\n</tr>\n<tr>\n<td colspan=\"2\">Total</td>\n</tr>\n</table>\n");
    expect("|===\n|*strong* |a:AsciiDoc _cell_\n|===\n",
           "<table>\n<tr>\n<td><strong>strong</strong></td>\n<td>AsciiDoc <em>cell</em></td>\n"
           "</tr>\n</table>\n");
    expect("++++\n<span class=\"raw\">kept</span>\n++++\n",
           "<span class=\"raw\">kept</span>\n");
    expect("= Conditional\n:feature:\n\nifdef::feature[]\nShown\nendif::[]\n"
           "ifndef::feature[]\nHidden\nendif::[]\n",
           "<div id=\"header\">\n<h1>Conditional</h1>\n</div>\n"
           "<div class=\"paragraph\">\n<p>Shown</p>\n</div>\n");
    expect("Press kbd:[Ctrl+C], choose btn:[Save], then menu:[File > Open]. footnote:[A note]\n",
           "<div class=\"paragraph\">\n<p>Press <kbd>Ctrl+C</kbd>, choose <b class=\"button\">Save</b>, "
           "then <span class=\"menuseq\">File &gt; Open</span>. "
           "<span class=\"footnote\">A note</span></p>\n</div>\n");

    {
        markup::Options options;
        std::vector<std::string> dependencies;
        options.asciidoc_source_identity = "root.adoc";
        options.asciidoc_dependency = [&](const std::string& identity) { dependencies.push_back(identity); };
        options.asciidoc_include_resolver = [](
            const std::string& from, const std::string& target, std::string& content,
            std::string& canonical, std::string& resolver_error) {
            (void)resolver_error;
            if (from == "root.adoc" && target == "part.adoc") {
                content = "included *content*";
                canonical = "docs/part.adoc";
                return true;
            }
            return false;
        };
        std::string included, include_error;
        if (!markup::convert(markup::Format::AsciiDoc, "before\n\ninclude::part.adoc[]\n\nafter",
                             included, include_error, options) ||
            included != "<div class=\"paragraph\">\n<p>before</p>\n</div>\n"
                        "<div class=\"paragraph\">\n<p>included <strong>content</strong></p>\n</div>\n"
                        "<div class=\"paragraph\">\n<p>after</p>\n</div>\n" ||
            dependencies != std::vector<std::string>{"docs/part.adoc"}) return 4;
        ++checks;
    }
    {
        markup::Options safe;
        safe.allow_raw_html = false;
        std::string output, safe_error;
        if (!markup::convert(markup::Format::AsciiDoc,
                             "++++\n<script>alert(1)</script>\n++++\n\n"
                             "link:javascript:alert(1)[bad] pass:[<b>raw</b>]",
                             output, safe_error, safe) ||
            output.find("<script>") != std::string::npos ||
            output.find("href=\"javascript:") != std::string::npos ||
            output.find("<b>raw</b>") != std::string::npos) return 6;
        ++checks;
    }
    {
        std::string hostile;
        for (int i = 0; i < 5000; ++i) hostile += "* item **unterminated__ link:http://x[\n";
        std::string output, hostile_error;
        if (!markup::convert(markup::Format::AsciiDoc, hostile, output, hostile_error) ||
            output.size() > hostile.size() * 16U) return 7;
        ++checks;
    }
    {
        std::string ignored, include_error;
        if (markup::convert(markup::Format::AsciiDoc, "include::missing.adoc[]", ignored, include_error) ||
            include_error.find("requires a host resolver") == std::string::npos) return 5;
        ++checks;
    }
    {
        std::string ignored, reference_error;
        std::vector<std::string> diagnostics;
        markup::Options options;
        options.asciidoc_diagnostic = [&](const std::string& value) { diagnostics.push_back(value); };
        if (!markup::convert(markup::Format::AsciiDoc, "See <<missing>>.\n",
                             ignored, reference_error, options) || diagnostics.empty() ||
            diagnostics.back().find("unresolved cross-reference: missing") == std::string::npos) return 8;
        diagnostics.clear();
        if (!markup::convert(markup::Format::AsciiDoc,
                             "[[same]]\nfirst\n\n[[same]]\nsecond\n",
                             ignored, reference_error, options) || diagnostics.empty() ||
            diagnostics.back().find("duplicate anchor: same") == std::string::npos) return 9;
        diagnostics.clear();
        markup::Options identified;
        identified.asciidoc_source_identity = "root.adoc";
        identified.asciidoc_diagnostic = [&](const std::string& value) { diagnostics.push_back(value); };
        if (!markup::convert(markup::Format::AsciiDoc,
                             "[[same]]\nfirst\n\n[[same]]\nsecond\n",
                             ignored, reference_error, identified) || diagnostics.empty() ||
            diagnostics.back().find("root.adoc:5: duplicate anchor: same") == std::string::npos) return 10;
        checks += 3;
    }
    {
        // AsciiDoc include cycle detection via the canonical-identity stack.
        std::string output, cycle_error;
        markup::Options options;
        options.asciidoc_include_resolver = [](
            const std::string& from, const std::string&, std::string& content,
            std::string& canonical, std::string&) {
            content = "include::back.adoc[]\n";
            canonical = (from == "root.adoc") ? "docs/a.adoc" : "root.adoc";
            return true;
        };
        if (markup::convert(markup::Format::AsciiDoc, "include::a.adoc[]\n",
                            output, cycle_error, options) ||
            cycle_error.find("include cycle") == std::string::npos) return 11;
        ++checks;
    }
    {
        // AsciiDoc include depth limit (32 levels).
        std::string output, depth_error;
        markup::Options options;
        int depth = 0;
        options.asciidoc_include_resolver = [&](
            const std::string&, const std::string&, std::string& content,
            std::string& canonical, std::string&) {
            content = "include::next.adoc[]\n";
            canonical = "docs/depth" + std::to_string(++depth) + ".adoc";
            return true;
        };
        if (markup::convert(markup::Format::AsciiDoc, "include::depth1.adoc[]\n",
                            output, depth_error, options) ||
            depth_error.find("include depth exceeds 32") == std::string::npos) return 12;
        ++checks;
    }
    {
        // Post-selection expansion limit: indent must be clamped so the selected
        // output cannot exceed the documented 64 MiB budget.
        std::string output, indent_error;
        markup::Options options;
        options.asciidoc_include_resolver = [](
            const std::string&, const std::string&, std::string& content,
            std::string& canonical, std::string&) {
            content = "line\n";
            canonical = "docs/tiny.adoc";
            return true;
        };
        if (!markup::convert(markup::Format::AsciiDoc,
                             "include::tiny.adoc[indent=4294967295]\n",
                             output, indent_error, options) ||
            output.size() > 4096) return 13;
        ++checks;
    }
    {
        // A genuinely oversized include must still fail the expansion limit.
        std::string output, size_error;
        markup::Options options;
        std::string huge(64U * 1024U * 1024U, 'x');
        options.asciidoc_include_resolver = [&](
            const std::string&, const std::string&, std::string& content,
            std::string& canonical, std::string&) {
            content = huge;
            canonical = "docs/huge.adoc";
            return true;
        };
        if (markup::convert(markup::Format::AsciiDoc, "include::huge.adoc[]\n",
                            output, size_error, options) ||
            size_error.find("expanded input exceeds 64 MiB") == std::string::npos) return 14;
        ++checks;
    }
    {
        // Table-cell count limit is enforced without runaway allocation.
        std::string cells;
        for (int i = 0; i < 200000; ++i) cells += "|c ";
        std::string output, table_error;
        if (!markup::convert(markup::Format::AsciiDoc, "|===\n" + cells + "\n|===\n",
                             output, table_error)) return 15;
        ++checks;
    }

    std::string output, error;
    if (!markup::is_supported(markup::Format::AsciiDoc) ||
        !markup::convert(markup::Format::AsciiDoc, "repeat", output, error)) return 2;
    const std::string first = output;
    if (!markup::convert(markup::Format::AsciiDoc, "repeat", output, error) || output != first) return 3;
    checks += 2;
    std::cout << checks << " AsciiDoc model/invariant checks passed\n";
}
