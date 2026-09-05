#include "AsciiDoc.h"

#include <markup/Markup.h>

#include <cstddef>
#include <cctype>
#include <utility>
#include <set>

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

bool boundary_character(char character) {
    const unsigned char value = static_cast<unsigned char>(character);
    return std::isspace(value) || std::ispunct(value);
}

bool starts_with(const std::string& value, const std::string& prefix) {
    return value.compare(0, prefix.size(), prefix) == 0;
}

std::vector<Inline> parse_inlines(const std::string& text, const Range& source) {
    std::vector<Inline> output;
    std::string plain;
    auto flush = [&]() {
        if (plain.empty()) return;
        Inline node;
        node.text = std::move(plain);
        node.source = source;
        output.push_back(std::move(node));
        plain.clear();
    };
    struct Delimiter { const char* marker; InlineKind kind; bool constrained; };
    static const Delimiter delimiters[] = {
        {"**", InlineKind::Strong, false}, {"__", InlineKind::Emphasis, false},
        {"``", InlineKind::Monospace, false}, {"##", InlineKind::Mark, false},
        {"*", InlineKind::Strong, true}, {"_", InlineKind::Emphasis, true},
        {"`", InlineKind::Monospace, true}, {"#", InlineKind::Mark, true},
        {"^", InlineKind::Superscript, true}, {"~", InlineKind::Subscript, true},
    };

    for (std::size_t i = 0; i < text.size();) {
        if (text[i] == '\\' && i + 1 < text.size()) {
            plain.push_back(text[i + 1]);
            i += 2;
            continue;
        }
        const auto macro = [&](const std::string& prefix, InlineKind kind,
                               std::size_t at, Inline& node, std::size_t& next) {
            if (!starts_with(text.substr(at), prefix)) return false;
            const std::size_t target_start = at + prefix.size();
            const auto bracket = text.find('[', target_start);
            if (bracket == std::string::npos || bracket == target_start) return false;
            const auto close = text.find(']', bracket + 1);
            if (close == std::string::npos) return false;
            node.kind = kind;
            node.target = text.substr(target_start, bracket - target_start);
            node.text = text.substr(bracket + 1, close - bracket - 1);
            const auto comma = node.text.find(',');
            if (comma != std::string::npos) {
                node.title = trim(node.text.substr(comma + 1));
                node.text = trim(node.text.substr(0, comma));
            }
            node.source = source;
            if (kind == InlineKind::Link || kind == InlineKind::CrossReference)
                node.children = parse_inlines(node.text.empty() ? node.target : node.text, source);
            next = close + 1;
            return true;
        };
        Inline macro_node;
        std::size_t macro_next = i;
        if (macro("link:", InlineKind::Link, i, macro_node, macro_next) ||
            macro("mailto:", InlineKind::Link, i, macro_node, macro_next) ||
            macro("image:", InlineKind::Image, i, macro_node, macro_next) ||
            macro("icon:", InlineKind::Image, i, macro_node, macro_next) ||
            macro("xref:", InlineKind::CrossReference, i, macro_node, macro_next)) {
            flush();
            if (starts_with(text.substr(i), "mailto:")) macro_node.target = "mailto:" + macro_node.target;
            if (starts_with(text.substr(i), "icon:")) macro_node.title = "icon";
            output.push_back(std::move(macro_node));
            i = macro_next;
            continue;
        }
        if (text.compare(i, 2, "<<") == 0) {
            const auto close = text.find(">>", i + 2);
            if (close != std::string::npos) {
                flush();
                Inline xref;
                xref.kind = InlineKind::CrossReference;
                std::string inside = text.substr(i + 2, close - i - 2);
                const auto comma = inside.find(',');
                xref.target = trim(inside.substr(0, comma));
                xref.text = comma == std::string::npos ? xref.target : trim(inside.substr(comma + 1));
                xref.children = parse_inlines(xref.text, source);
                xref.source = source;
                output.push_back(std::move(xref));
                i = close + 2;
                continue;
            }
        }
        if (starts_with(text.substr(i), "http://") || starts_with(text.substr(i), "https://")) {
            std::size_t end = i;
            while (end < text.size() && !std::isspace(static_cast<unsigned char>(text[end]))) ++end;
            flush();
            Inline link;
            link.kind = InlineKind::Link;
            link.target = text.substr(i, end - i);
            link.text = link.target;
            Inline label;
            label.text = link.text;
            label.source = source;
            link.children.push_back(std::move(label));
            link.source = source;
            output.push_back(std::move(link));
            i = end;
            continue;
        }
        if (text.compare(i, 3, "(C)") == 0) { plain += "\xc2\xa9"; i += 3; continue; }
        if (text.compare(i, 3, "(R)") == 0) { plain += "\xc2\xae"; i += 3; continue; }
        if (text.compare(i, 4, "(TM)") == 0) { plain += "\xe2\x84\xa2"; i += 4; continue; }
        if (text.compare(i, 4, " -> ") == 0) { plain += " \xe2\x86\x92 "; i += 4; continue; }
        if (text.compare(i, 4, " <- ") == 0) { plain += " \xe2\x86\x90 "; i += 4; continue; }
        if (text[i] == '\n' && !plain.empty() && plain.size() >= 2 &&
            plain[plain.size() - 1] == '+' && plain[plain.size() - 2] == ' ') {
            plain.resize(plain.size() - 2);
            flush();
            Inline line_break;
            line_break.kind = InlineKind::LineBreak;
            line_break.source = source;
            output.push_back(std::move(line_break));
            ++i;
            continue;
        }

        bool matched = false;
        for (const auto& delimiter : delimiters) {
            const std::string marker = delimiter.marker;
            if (text.compare(i, marker.size(), marker) != 0) continue;
            if (delimiter.constrained && i > 0 && !boundary_character(text[i - 1])) continue;
            if (i + marker.size() >= text.size() ||
                std::isspace(static_cast<unsigned char>(text[i + marker.size()]))) continue;
            std::size_t end = text.find(marker, i + marker.size());
            while (end != std::string::npos && delimiter.constrained &&
                   end + marker.size() < text.size() &&
                   !boundary_character(text[end + marker.size()])) {
                end = text.find(marker, end + marker.size());
            }
            if (end == std::string::npos || end == i + marker.size()) continue;
            flush();
            Inline span;
            span.kind = delimiter.kind;
            span.text = text.substr(i + marker.size(), end - i - marker.size());
            span.source = source;
            if (span.kind != InlineKind::Monospace) span.children = parse_inlines(span.text, source);
            output.push_back(std::move(span));
            i = end + marker.size();
            matched = true;
            break;
        }
        if (matched) continue;
        plain.push_back(text[i++]);
    }
    flush();
    return output;
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
    block.inlines = parse_inlines(block.text, block.source);
}

bool delimited_block(const std::string& line, BlockKind& kind) {
    if (line == "----") kind = BlockKind::Listing;
    else if (line == "....") kind = BlockKind::Literal;
    else if (line == "--") kind = BlockKind::Open;
    else if (line == "====") kind = BlockKind::Example;
    else if (line == "****") kind = BlockKind::Sidebar;
    else if (line == "____") kind = BlockKind::Quote;
    else if (line == "////") kind = BlockKind::Comment;
    else return false;
    return true;
}

bool verbatim_kind(BlockKind kind) {
    return kind == BlockKind::Listing || kind == BlockKind::Literal ||
           kind == BlockKind::Source || kind == BlockKind::Verse ||
           kind == BlockKind::Comment;
}

void parse_block_attributes(const std::string& line, std::string& style) {
    if (line.size() < 2 || line.front() != '[' || line.back() != ']') return;
    const std::string inside = line.substr(1, line.size() - 2);
    const auto comma = inside.find(',');
    style = trim(inside.substr(0, comma));
}

std::string anchor_id(const std::string& line) {
    if (line.size() > 4 && starts_with(line, "[[") && line.substr(line.size() - 2) == "]] ") return {};
    if (line.size() > 4 && starts_with(line, "[[") && line.substr(line.size() - 2) == "]]" )
        return trim(line.substr(2, line.size() - 4));
    if (line.size() > 3 && starts_with(line, "[#") && line.back() == ']')
        return trim(line.substr(2, line.size() - 3));
    return {};
}

struct ListInfo {
    BlockKind kind = BlockKind::Paragraph;
    std::size_t depth = 0;
    unsigned start = 1;
    std::string marker;
    std::string principal;
};

bool list_info(const std::string& value, ListInfo& info) {
    std::size_t count = 0;
    while (count < value.size() && value[count] == '*') ++count;
    if (count && count <= 64 && count < value.size() && value[count] == ' ') {
        info.kind = BlockKind::UnorderedList;
        info.depth = count;
        info.marker = value.substr(0, count);
        info.principal = value.substr(count + 1);
        return true;
    }
    count = 0;
    while (count < value.size() && value[count] == '.') ++count;
    if (count && count <= 64 && count < value.size() && value[count] == ' ') {
        info.kind = BlockKind::OrderedList;
        info.depth = count;
        info.marker = value.substr(0, count);
        info.principal = value.substr(count + 1);
        return true;
    }
    std::size_t digits = 0;
    while (digits < value.size() && std::isdigit(static_cast<unsigned char>(value[digits]))) ++digits;
    if (digits && digits <= 9 && digits + 1 < value.size() && value[digits] == '.' && value[digits + 1] == ' ') {
        info.kind = BlockKind::OrderedList;
        info.depth = 1;
        info.start = static_cast<unsigned>(std::stoul(value.substr(0, digits)));
        info.marker = value.substr(0, digits + 1);
        info.principal = value.substr(digits + 2);
        return true;
    }
    const auto description = value.find("::");
    if (description != std::string::npos && description > 0 &&
        (description + 2 == value.size() || value[description + 2] == ' ')) {
        info.kind = BlockKind::DescriptionList;
        info.depth = 1;
        info.marker = "::";
        info.principal = value.substr(0, description);
        if (description + 2 < value.size()) {
            info.principal += '\n';
            info.principal += value.substr(description + 3);
        }
        return true;
    }
    return false;
}

void parse_list(const std::vector<std::string>& lines, std::size_t& line,
                std::size_t depth, BlockKind kind, Block& list,
                const std::map<std::string, std::string>& attributes) {
    list.kind = kind;
    list.source.begin = {line + 1, 1};
    while (line < lines.size()) {
        ListInfo info;
        if (!list_info(lines[line], info) || info.kind != kind || info.depth != depth) break;
        if (list.items.empty()) {
            list.marker = info.marker;
            list.start = info.start;
        }
        const std::size_t first = line++;
        Block item;
        item.kind = BlockKind::Paragraph;
        item.marker = info.marker;
        item.start = info.start;
        std::string principal = info.principal;
        if (kind == BlockKind::DescriptionList) {
            const auto newline = principal.find('\n');
            item.title = principal.substr(0, newline);
            principal = newline == std::string::npos ? "" : principal.substr(newline + 1);
        }
        if (principal.size() >= 3 && principal.front() == '[' && principal[2] == ']' &&
            (principal[1] == ' ' || principal[1] == 'x' || principal[1] == 'X')) {
            item.checklist = true;
            item.checked = principal[1] != ' ';
            list.checklist = true;
            principal = trim(principal.substr(3));
        }
        add_text_inline(item, principal, first, first, attributes);

        while (line < lines.size()) {
            ListInfo nested;
            if (!list_info(lines[line], nested) || nested.depth <= depth) break;
            Block child;
            parse_list(lines, line, nested.depth, nested.kind, child, attributes);
            item.blocks.push_back(std::move(child));
        }
        if (line < lines.size() && lines[line] == "+") {
            ++line;
            while (line < lines.size() && lines[line].empty()) ++line;
            if (line < lines.size()) {
                const std::size_t continuation_line = line++;
                Block continuation;
                add_text_inline(continuation, lines[continuation_line], continuation_line,
                                continuation_line, attributes);
                item.blocks.push_back(std::move(continuation));
            }
        }
        item.source.begin = {first + 1, 1};
        const std::size_t last = line ? line - 1 : first;
        item.source.end = {last + 1, lines[last].size()};
        list.items.push_back(std::move(item));
    }
    const std::size_t last = line ? line - 1 : 0;
    list.source.end = {last + 1, lines[last].size()};
}

void parse_blocks(const std::vector<std::string>& lines, std::size_t& line,
                  unsigned parent_level, std::vector<Block>& blocks,
                  const std::map<std::string, std::string>& attributes,
                  const std::string& end_delimiter = {}, std::size_t nesting = 0) {
    std::string pending_title;
    std::string pending_style;
    std::string pending_id;
    while (line < lines.size()) {
        while (line < lines.size() && lines[line].empty()) ++line;
        if (line == lines.size()) return;
        if (!end_delimiter.empty() && lines[line] == end_delimiter) {
            ++line;
            return;
        }
        const std::string id = anchor_id(lines[line]);
        if (!id.empty()) { pending_id = id; ++line; continue; }

        if (lines[line].size() > 1 && lines[line].front() == '.' && lines[line][1] != '.') {
            pending_title = substitute_attributes(lines[line++].substr(1), attributes);
            continue;
        }
        if (lines[line].size() > 1 && lines[line].front() == '[' && lines[line].back() == ']') {
            parse_block_attributes(lines[line++], pending_style);
            continue;
        }

        std::string title;
        const unsigned level = section_level(lines[line], title);
        if (level) {
            if (parent_level && level <= parent_level) return;
            const std::size_t first = line++;
            Block section;
            section.kind = BlockKind::Section;
            section.level = level;
            section.id = pending_id;
            section.title = substitute_attributes(title, attributes);
            section.source.begin = {first + 1, 1};
            parse_blocks(lines, line, level, section.blocks, attributes, end_delimiter, nesting);
            const std::size_t last = line ? line - 1 : first;
            section.source.end = {last + 1, lines[last].size()};
            section.title = substitute_attributes(section.title, attributes);
            blocks.push_back(std::move(section));
            pending_id.clear();
            continue;
        }

        if (lines[line] == "'''" || lines[line] == "<<<") {
            Block separator;
            separator.kind = lines[line] == "'''" ? BlockKind::ThematicBreak : BlockKind::PageBreak;
            separator.source.begin = separator.source.end = {line + 1, lines[line].size()};
            ++line;
            blocks.push_back(std::move(separator));
            pending_title.clear(); pending_style.clear();
            continue;
        }

        BlockKind delimiter_kind;
        if (delimited_block(lines[line], delimiter_kind)) {
            const std::string delimiter = lines[line];
            const std::size_t first = line++;
            Block block;
            block.kind = delimiter_kind;
            block.title = pending_title;
            block.style = pending_style;
            block.id = pending_id;
            if (pending_style == "source" && delimiter_kind == BlockKind::Listing) block.kind = BlockKind::Source;
            if (pending_style == "verse" && delimiter_kind == BlockKind::Quote) block.kind = BlockKind::Verse;
            block.source.begin = {first + 1, 1};
            if (verbatim_kind(block.kind)) {
                std::string text;
                const std::size_t content_first = line;
                while (line < lines.size() && lines[line] != delimiter) {
                    if (!text.empty()) text += '\n';
                    text += lines[line++];
                }
                block.text = substitute_attributes(text, attributes);
                if (!block.text.empty()) {
                    Inline literal;
                    literal.text = block.text;
                    literal.source.begin = {content_first + 1, 1};
                    literal.source.end = {line, lines[line - 1].size()};
                    block.inlines.push_back(std::move(literal));
                }
                if (line < lines.size()) ++line;
            } else if (nesting < 64) {
                parse_blocks(lines, line, parent_level, block.blocks, attributes, delimiter, nesting + 1);
            } else {
                while (line < lines.size() && lines[line] != delimiter) {
                    if (!block.text.empty()) block.text += '\n';
                    block.text += lines[line++];
                }
                if (line < lines.size()) ++line;
            }
            const std::size_t last = line ? line - 1 : first;
            block.source.end = {last + 1, lines[last].size()};
            blocks.push_back(std::move(block));
            pending_title.clear(); pending_style.clear();
            pending_id.clear();
            continue;
        }

        ListInfo first_item;
        if (list_info(lines[line], first_item)) {
            Block list;
            list.title = pending_title;
            list.style = pending_style;
            list.id = pending_id;
            parse_list(lines, line, first_item.depth, first_item.kind, list, attributes);
            blocks.push_back(std::move(list));
            pending_title.clear(); pending_style.clear();
            pending_id.clear();
            continue;
        }

        if (!lines[line].empty() && (lines[line][0] == ' ' || lines[line][0] == '\t')) {
            const std::size_t first = line;
            Block literal;
            literal.kind = BlockKind::Literal;
            literal.title = pending_title;
            literal.id = pending_id;
            while (line < lines.size() && (lines[line].empty() || lines[line][0] == ' ' || lines[line][0] == '\t')) {
                if (!literal.text.empty()) literal.text += '\n';
                literal.text += lines[line].empty() ? "" :
                    (lines[line][0] == '\t' ? lines[line].substr(1) : lines[line].substr(1));
                ++line;
            }
            literal.source.begin = {first + 1, 1};
            literal.source.end = {line, lines[line - 1].size()};
            blocks.push_back(std::move(literal));
            pending_title.clear(); pending_style.clear();
            pending_id.clear();
            continue;
        }

        const std::size_t first = line;
        std::string text = lines[line++];
        while (line < lines.size() && !lines[line].empty()) {
            std::string next_title;
            if (section_level(lines[line], next_title)) break;
            BlockKind next_kind;
            if (delimited_block(lines[line], next_kind) || lines[line] == "'''" ||
                lines[line] == "<<<" || (!end_delimiter.empty() && lines[line] == end_delimiter) ||
                (lines[line].size() > 1 && lines[line].front() == '[' && lines[line].back() == ']') ||
                (lines[line].size() > 1 && lines[line].front() == '.' && lines[line][1] != '.')) break;
            ListInfo next_list;
            if (list_info(lines[line], next_list)) break;
            text += '\n';
            text += lines[line++];
        }
        Block paragraph;
        add_text_inline(paragraph, text, first, line - 1, attributes);
        paragraph.title = pending_title;
        paragraph.style = pending_style;
        paragraph.id = pending_id;
        blocks.push_back(std::move(paragraph));
        pending_title.clear(); pending_style.clear();
        pending_id.clear();
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

bool safe_uri(const std::string& uri) {
    std::string scheme;
    for (unsigned char c : uri) if (!std::isspace(c)) scheme.push_back(static_cast<char>(std::tolower(c)));
    return !starts_with(scheme, "javascript:") && !starts_with(scheme, "vbscript:") &&
           !starts_with(scheme, "data:");
}

std::string escape_attribute(const std::string& value) {
    std::string output = escape_html(value);
    std::string result;
    for (char c : output) result += c == '"' ? "&quot;" : std::string(1, c);
    return result;
}

std::string render_inlines(const std::vector<Inline>& inlines, const Options& options) {
    std::string output;
    for (const auto& node : inlines) {
        switch (node.kind) {
        case InlineKind::Text: output += escape_html(node.text); break;
        case InlineKind::Strong: output += "<strong>" + render_inlines(node.children, options) + "</strong>"; break;
        case InlineKind::Emphasis: output += "<em>" + render_inlines(node.children, options) + "</em>"; break;
        case InlineKind::Monospace: output += "<code>" + escape_html(node.text) + "</code>"; break;
        case InlineKind::Mark: output += "<mark>" + render_inlines(node.children, options) + "</mark>"; break;
        case InlineKind::Superscript: output += "<sup>" + render_inlines(node.children, options) + "</sup>"; break;
        case InlineKind::Subscript: output += "<sub>" + render_inlines(node.children, options) + "</sub>"; break;
        case InlineKind::Link: {
            const std::string target = !options.allow_raw_html && !safe_uri(node.target) ? "" : node.target;
            output += "<a href=\"" + escape_attribute(target) + "\">" +
                      render_inlines(node.children, options) + "</a>";
            break;
        }
        case InlineKind::CrossReference:
            output += "<a href=\"#" + escape_attribute(node.target) + "\">" +
                      render_inlines(node.children, options) + "</a>"; break;
        case InlineKind::Image: {
            const std::string target = !options.allow_raw_html && !safe_uri(node.target) ? "" : node.target;
            if (node.title == "icon") output += "<span class=\"icon\">" + escape_html(node.text) + "</span>";
            else output += "<img src=\"" + escape_attribute(target) + "\" alt=\"" +
                           escape_attribute(node.text) + "\">";
            break;
        }
        case InlineKind::LineBreak: output += "<br>\n"; break;
        default: output += escape_html(node.text); break;
        }
    }
    return output;
}

std::string render_inline_text(const std::string& value, const Options& options) {
    return render_inlines(parse_inlines(value, {}), options);
}

std::string id_attribute(const Block& block) {
    return block.id.empty() ? "" : " id=\"" + escape_attribute(block.id) + "\"";
}

std::string render_blocks(const std::vector<Block>& blocks, const Options& options) {
    std::string output;
    for (const auto& block : blocks) {
        if (block.kind == BlockKind::Paragraph) {
            if (!block.title.empty()) output += "<div class=\"title\">" + escape_html(block.title) + "</div>\n";
            output += "<div class=\"paragraph\"" + id_attribute(block) + ">\n<p>" + render_inlines(block.inlines, options) +
                      "</p>\n</div>\n";
        } else if (block.kind == BlockKind::Section) {
            output += "<div class=\"sect" + std::to_string(block.level) + "\"" + id_attribute(block) + ">\n<h" +
                      std::to_string(block.level + 1) + ">" + render_inline_text(block.title, options) + "</h" +
                      std::to_string(block.level + 1) + ">\n";
            output += render_blocks(block.blocks, options);
            output += "</div>\n";
        } else if (block.kind == BlockKind::Listing || block.kind == BlockKind::Literal ||
                   block.kind == BlockKind::Source) {
            const std::string role = block.kind == BlockKind::Source ? "source" :
                                     block.kind == BlockKind::Listing ? "listingblock" : "literalblock";
            output += "<div class=\"" + role + "\"" + id_attribute(block) + ">\n";
            if (!block.title.empty()) output += "<div class=\"title\">" + escape_html(block.title) + "</div>\n";
            output += "<pre>" + escape_html(block.text) + "</pre>\n</div>\n";
        } else if (block.kind == BlockKind::Sidebar || block.kind == BlockKind::Example ||
                   block.kind == BlockKind::Open) {
            const std::string role = block.kind == BlockKind::Sidebar ? "sidebarblock" :
                                     block.kind == BlockKind::Example ? "exampleblock" : "openblock";
            output += "<div class=\"" + role + "\">\n";
            if (!block.title.empty()) output += "<div class=\"title\">" + escape_html(block.title) + "</div>\n";
            output += render_blocks(block.blocks, options) + "</div>\n";
        } else if (block.kind == BlockKind::Quote || block.kind == BlockKind::Verse) {
            output += "<blockquote";
            if (block.kind == BlockKind::Verse) output += " class=\"verse\"";
            output += ">\n" + render_blocks(block.blocks, options) + "</blockquote>\n";
        } else if (block.kind == BlockKind::ThematicBreak) {
            output += "<hr>\n";
        } else if (block.kind == BlockKind::PageBreak) {
            output += "<div class=\"pagebreak\"></div>\n";
        } else if (block.kind == BlockKind::UnorderedList || block.kind == BlockKind::OrderedList) {
            const bool ordered = block.kind == BlockKind::OrderedList;
            output += ordered ? "<ol" : "<ul";
            if (ordered && block.start != 1) output += " start=\"" + std::to_string(block.start) + "\"";
            if (block.checklist) output += " class=\"checklist\"";
            output += ">\n";
            for (const auto& item : block.items) {
                output += "<li>";
                if (item.checklist) {
                    output += "<input type=\"checkbox\" disabled";
                    if (item.checked) output += " checked";
                    output += "> ";
                }
                output += render_inlines(item.inlines, options);
                if (!item.blocks.empty()) output += '\n' + render_blocks(item.blocks, options);
                output += "</li>\n";
            }
            output += ordered ? "</ol>\n" : "</ul>\n";
        } else if (block.kind == BlockKind::DescriptionList) {
            output += "<dl>\n";
            for (const auto& item : block.items) {
                output += "<dt>" + escape_html(item.title) + "</dt>\n<dd>" +
                          render_inlines(item.inlines, options);
                if (!item.blocks.empty()) output += '\n' + render_blocks(item.blocks, options);
                output += "</dd>\n";
            }
            output += "</dl>\n";
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

std::string render_html(const Document& document, const Options& options) {
    std::string output;
    if (!document.title.empty()) {
        output += "<div id=\"header\">\n<h1>" + render_inline_text(document.title, options) + "</h1>\n";
        if (!document.author.empty()) output += "<div class=\"details\">" + escape_html(document.author) + "</div>\n";
        output += "</div>\n";
    }
    output += render_blocks(document.blocks, options);
    return output;
}

} // namespace markup::asciidoc
