#include "AsciiDoc.h"

#include <markup/Markup.h>

#include <cstddef>
#include <utility>

namespace markup::asciidoc {
namespace {

constexpr std::size_t max_input_bytes = 64U * 1024U * 1024U;

std::string normalize(const std::string& input) {
    std::string output;
    output.reserve(input.size());
    for (std::size_t i = 0; i < input.size(); ++i) {
        const unsigned char byte = static_cast<unsigned char>(input[i]);
        if (byte == 0) {
            output += "\xef\xbf\xbd";
        } else if (byte == '\r') {
            output.push_back('\n');
            if (i + 1 < input.size() && input[i + 1] == '\n') ++i;
        } else {
            output.push_back(static_cast<char>(byte));
        }
    }
    return output;
}

std::vector<std::string> split_lines(const std::string& input) {
    std::vector<std::string> result;
    std::size_t start = 0;
    while (start < input.size()) {
        const auto end = input.find('\n', start);
        result.push_back(input.substr(start, end == std::string::npos
            ? std::string::npos : end - start));
        if (end == std::string::npos) break;
        start = end + 1;
    }
    return result;
}

std::string escape_html(const std::string& value) {
    std::string output;
    output.reserve(value.size());
    for (const char character : value) {
        switch (character) {
        case '&': output += "&amp;"; break;
        case '<': output += "&lt;"; break;
        case '>': output += "&gt;"; break;
        default: output.push_back(character); break;
        }
    }
    return output;
}

std::string render_inlines(const std::vector<Inline>& inlines) {
    std::string output;
    for (const auto& inline_node : inlines) output += escape_html(inline_node.text);
    return output;
}

} // namespace

bool parse(const std::string& input, Document& document, std::string& error) {
    document = {};
    error.clear();
    if (input.size() > max_input_bytes) {
        error = "AsciiDoc input exceeds the 64 MiB library limit";
        return false;
    }

    const auto input_lines = split_lines(normalize(input));
    std::size_t line = 0;
    while (line < input_lines.size()) {
        while (line < input_lines.size() && input_lines[line].empty()) ++line;
        if (line == input_lines.size()) break;
        const std::size_t first = line;
        std::string text = input_lines[line++];
        while (line < input_lines.size() && !input_lines[line].empty()) {
            text += '\n';
            text += input_lines[line++];
        }
        Block paragraph;
        paragraph.text = text;
        paragraph.source.begin = {first + 1, 1};
        paragraph.source.end = {line, input_lines[line - 1].size()};
        Inline inline_text;
        inline_text.text = text;
        inline_text.source = paragraph.source;
        paragraph.inlines.push_back(std::move(inline_text));
        document.blocks.push_back(std::move(paragraph));
    }
    if (!input_lines.empty()) {
        document.source.begin = {1, 1};
        document.source.end = {input_lines.size(), input_lines.back().size()};
    }
    return true;
}

std::string render_html(const Document& document, const Options&) {
    std::string output;
    for (const auto& block : document.blocks) {
        if (block.kind == BlockKind::Paragraph) {
            output += "<div class=\"paragraph\">\n<p>" + render_inlines(block.inlines) +
                      "</p>\n</div>\n";
        }
    }
    return output;
}

} // namespace markup::asciidoc
