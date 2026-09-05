#include <markup/Markup.h>

#include <cstdlib>
#include <iostream>
#include <string>

namespace {
int checks = 0;

std::string render(const std::string& source, markup::Options options = {}) {
    std::string output, error;
    if (!markup::convert(markup::Format::Markdown, source, output, error, options)) {
        std::cerr << "conversion failed: " << error << '\n';
        std::exit(1);
    }
    ++checks;
    return output;
}

void require(bool value, const char* message) {
    if (!value) {
        std::cerr << "FAIL " << message << '\n';
        std::exit(1);
    }
}
}

int main() {
    markup::Options core;
    markup::Options extended;
    extended.markdown_profile = markup::Options::MarkdownProfile::Extended;

    require(render("~~gone~~\n", core) == "<p>~~gone~~</p>\n", "core strikethrough isolation");
    require(render("~~gone~~\n", extended) == "<p><del>gone</del></p>\n", "extended strikethrough");
    require(render("- [x] done\n", core) == "<ul>\n<li>[x] done</li>\n</ul>\n", "core task isolation");
    require(render("- [x] done\n", extended).find("checkbox") != std::string::npos, "extended task");
    require(render("a | b\n--- | ---\n1 | 2\n", core).find("<table>") == std::string::npos, "core table isolation");
    require(render("a | b\n--- | ---\n1 | 2\n", extended).find("<table>") != std::string::npos, "extended table");

    for (auto profile : {markup::Options::MarkdownProfile::CommonMark,
                         markup::Options::MarkdownProfile::Extended}) {
        markup::Options raw;
        raw.markdown_profile = profile;
        require(render("<script>x</script>\n", raw).find("<script>") != std::string::npos,
                "raw HTML pass-through");

        markup::Options safe = raw;
        safe.allow_raw_html = false;
        require(render("<script>x</script>\n", safe).find("<script>") == std::string::npos,
                "safe HTML boundary");

        markup::Options standalone = raw;
        standalone.standalone = true;
        require(render("# title\n", standalone).rfind("<!doctype html>", 0) == 0,
                "standalone wrapper");
    }

    static_assert(markup::api_version == 4, "RST host capabilities require API version 4");
    std::cout << checks << " profile matrix conversions passed\n";
}
