#include "AsciiDoc.h"

#include <markup/Markup.h>

#include <cstddef>
#include <cctype>
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

std::string trim(const std::string& value) {
    const auto first = value.find_first_not_of(" \t");
    if (first == std::string::npos) return {};
    const auto last = value.find_last_not_of(" \t");
    return value.substr(first, last - first + 1);
}

std::string substitute_attributes(const std::string& value,
                                  const std::map<std::string, std::string>& attributes) {
    std::string result;
    for (std::size_t i = 0; i < value.size();) {
        if (value[i] == '\\' && i + 1 < value.size() && value[i + 1] == '{') {
            result.push_back('{');
            i += 2;
            continue;
        }
        if (value[i] == '{') {
            const auto end = value.find('}', i + 1);
            if (end != std::string::npos) {
                const auto found = attributes.find(value.substr(i + 1, end - i - 1));
                if (found != attributes.end()) {
                    result += found->second;
                    i = end + 1;
                    continue;
                }
            }
        }
        result.push_back(value[i++]);
    }
    return result;
}

bool attribute_entry(const std::string& line, std::string& name,
                     std::string& value, bool& unset) {
    if (line.size() < 3 || line.front() != ':') return false;
    const auto separator = line.find(':', 1);
    if (separator == std::string::npos) return false;
    name = line.substr(1, separator - 1);
    unset = false;
    if (!name.empty() && name.front() == '!') {
        name.erase(name.begin());
        unset = true;
    } else if (!name.empty() && name.back() == '!') {
        name.pop_back();
        unset = true;
    }
    if (name.empty()) return false;
    value = trim(line.substr(separator + 1));
    return true;
}

unsigned section_level(const std::string& line, std::string& title) {
    std::size_t equals = 0;
    while (equals < line.size() && line[equals] == '=') ++equals;
    if (equals < 2 || equals > 6 || equals >= line.size() || line[equals] != ' ') return 0;
    title = trim(line.substr(equals + 1));
    return static_cast<unsigned>(equals - 1);
}

void add_text_inline(Block& block, const std::string& text,
                     std::size_t first_line, std::size_t last_line,
                     const std::map<std::string, std::string>& attributes) {
    block.text = substitute_attributes(text, attributes);
    block.source.begin = {first_line + 1, 1};
    block.source.end = {last_line + 1, text.size()};
    Inline inline_text;
    inline_text.text = block.text;
    inline_text.source = block.source;
    block.inlines.push_back(std::move(inline_text));
}

void parse_blocks(const std::vector<std::string>& lines, std::size_t& line,
                  unsigned parent_level, std::vector<Block>& blocks,
                  const std::map<std::string, std::string>& attributes) {
    while (line < lines.size()) {
        while (line < lines.size() && lines[line].empty()) ++line;
        if (line == lines.size()) return;

        std::string title;
        const unsigned level = section_level(lines[line], title);
        if (level) {
            if (parent_level && level <= parent_level) return;
            const std::size_t first = line++;
            Block section;
            section.kind = BlockKind::Section;
            section.level = level;
            section.title = substitute_attributes(title, attributes);
            section.source.begin = {first + 1, 1};
            parse_blocks(lines, line, level, section.blocks, attributes);
            const std::size_t last = line ? line - 1 : first;
            section.source.end = {last + 1, lines[last].size()};
            blocks.push_back(std::move(section));
            continue;
        }

        const std::size_t first = line;
        std::string text = lines[line++];
        while (line < lines.size() && !lines[line].empty()) {
            std::string next_title;
            if (section_level(lines[line], next_title)) break;
            text += '\n';
            text += lines[line++];
        }
        Block paragraph;
        add_text_inline(paragraph, text, first, line - 1, attributes);
        blocks.push_back(std::move(paragraph));
    }
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

std::string render_blocks(const std::vector<Block>& blocks) {
    std::string output;
    for (const auto& block : blocks) {
        if (block.kind == BlockKind::Paragraph) {
            output += "<div class=\"paragraph\">\n<p>" + render_inlines(block.inlines) +
                      "</p>\n</div>\n";
        } else if (block.kind == BlockKind::Section) {
            output += "<div class=\"sect" + std::to_string(block.level) + "\">\n<h" +
                      std::to_string(block.level + 1) + ">" + escape_html(block.title) + "</h" +
                      std::to_string(block.level + 1) + ">\n";
            output += render_blocks(block.blocks);
            output += "</div>\n";
        }
    }
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
    if (!input_lines.empty() && input_lines[0].rfind("= ", 0) == 0) {
        document.title = trim(input_lines[0].substr(2));
        document.attributes["doctitle"] = document.title;
        line = 1;
        if (line < input_lines.size() && !input_lines[line].empty() && input_lines[line][0] != ':') {
            document.author = input_lines[line++];
            if (line < input_lines.size() && !input_lines[line].empty() && input_lines[line][0] != ':') {
                document.revision = input_lines[line++];
            }
        }
        while (line < input_lines.size()) {
            std::string name, value;
            bool unset = false;
            if (!attribute_entry(input_lines[line], name, value, unset)) break;
            if (unset) document.attributes.erase(name);
            else document.attributes[name] = substitute_attributes(value, document.attributes);
            ++line;
        }
        document.title = substitute_attributes(document.title, document.attributes);
        document.attributes["doctitle"] = document.title;
    }
    parse_blocks(input_lines, line, 0, document.blocks, document.attributes);
    if (!input_lines.empty()) {
        document.source.begin = {1, 1};
        document.source.end = {input_lines.size(), input_lines.back().size()};
    }
    return true;
}

std::string render_html(const Document& document, const Options&) {
    std::string output;
    if (!document.title.empty()) {
        output += "<div id=\"header\">\n<h1>" + escape_html(document.title) + "</h1>\n";
        if (!document.author.empty()) output += "<div class=\"details\">" + escape_html(document.author) + "</div>\n";
        output += "</div>\n";
    }
    output += render_blocks(document.blocks);
    return output;
}

} // namespace markup::asciidoc
