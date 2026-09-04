#include <markup/Markup.h>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <system_error>

namespace fs = std::filesystem;

namespace {

void help() {
    std::cout
        << "Markup++ - convert markup formats to HTML\n\n"
        << "Usage: markup [options] <input>\n\n"
        << "Options:\n"
        << "  -o, --output <file>   Write HTML atomically instead of stdout\n"
        << "  -f, --format <name>   Input format: markdown, asciidoc, rst\n"
        << "      --standalone      Emit a complete HTML document\n"
        << "      --title <text>    Set the standalone document title\n"
        << "      --safe            Escape raw HTML and reject active URLs\n"
        << "      --commonmark      Use CommonMark 0.31.2 (the default)\n"
        << "      --extensions      Enable legacy tables/tasks/strikethrough\n"
        << "  -h, --help            Show this help\n"
        << "  -v, --version         Show the version\n\n"
        << "Use '-' as the input to read from stdin. Markdown is implemented in\n"
        << "this checkpoint; AsciiDoc and reStructuredText are planned.\n";
}

bool parse_format(const std::string& name, markup::Format& format) {
    if (name == "markdown" || name == "md") { format = markup::Format::Markdown; return true; }
    if (name == "asciidoc" || name == "adoc") { format = markup::Format::AsciiDoc; return true; }
    if (name == "restructuredtext" || name == "rst") { format = markup::Format::ReStructuredText; return true; }
    return false;
}

bool read_stream(std::istream& stream, std::string& value) {
    std::ostringstream buffer;
    buffer << stream.rdbuf();
    if (stream.bad()) return false;
    value = buffer.str();
    return true;
}

bool read_file(const fs::path& path, std::string& value) {
    std::ifstream input(path, std::ios::binary);
    return input && read_stream(input, value);
}

class TemporaryDirectory {
public:
    explicit TemporaryDirectory(fs::path path) : path_(std::move(path)) {}
    ~TemporaryDirectory() {
        std::error_code ignored;
        fs::remove_all(path_, ignored);
    }
    const fs::path& path() const { return path_; }
private:
    fs::path path_;
};

bool make_temporary_directory(const fs::path& destination, fs::path& temporary,
                              std::error_code& error) {
    const fs::path parent = destination.has_parent_path() ? destination.parent_path() : fs::path(".");
    const std::string stem = "." + destination.filename().string() + ".markup-tmp-";
    const auto seed = std::chrono::steady_clock::now().time_since_epoch().count();
    for (unsigned attempt = 0; attempt < 128; ++attempt) {
        temporary = parent / (stem + std::to_string(seed) + "-" + std::to_string(attempt));
        error.clear();
        if (fs::create_directory(temporary, error)) return true;
        if (error && error != std::errc::file_exists) return false;
    }
    error = std::make_error_code(std::errc::file_exists);
    return false;
}

bool write_file(const fs::path& path, const std::string& value, std::string& error) {
    std::error_code ec;
    const fs::file_status status = fs::symlink_status(path, ec);
    if (ec && ec != std::errc::no_such_file_or_directory) { error = ec.message(); return false; }
    const bool exists = !ec && fs::exists(status);
    if (exists && fs::is_symlink(status)) { error = "refusing to replace a symbolic link"; return false; }
    if (exists && !fs::is_regular_file(status)) { error = "destination is not a regular file"; return false; }

    fs::path temporary_path;
    if (!make_temporary_directory(path, temporary_path, ec)) {
        error = "cannot create temporary directory: " + ec.message();
        return false;
    }
    TemporaryDirectory temporary(temporary_path);
    const fs::path candidate = temporary.path() / "output";
    {
        std::ofstream output(candidate, std::ios::binary | std::ios::trunc);
        if (!output) { error = "cannot create temporary output"; return false; }
        output.write(value.data(), static_cast<std::streamsize>(value.size()));
        output.close();
        if (!output) { error = "failed while writing temporary output"; return false; }
    }
    if (exists) {
        fs::permissions(candidate, status.permissions(), ec);
        if (ec) { error = "cannot preserve destination permissions: " + ec.message(); return false; }
    }
    fs::rename(candidate, path, ec);
    if (!ec) return true;
    if (!exists) { error = ec.message(); return false; }
    const fs::path previous = temporary.path() / "previous";
    ec.clear();
    fs::rename(path, previous, ec);
    if (ec) { error = "cannot prepare destination replacement: " + ec.message(); return false; }
    fs::rename(candidate, path, ec);
    if (!ec) return true;
    const std::string commit_error = ec.message();
    std::error_code restore_error;
    fs::rename(previous, path, restore_error);
    error = "cannot commit temporary output: " + commit_error;
    if (restore_error) error += "; cannot restore previous file: " + restore_error.message();
    return false;
}

} // namespace

int main(int argc, char** argv) {
    fs::path input_path;
    fs::path output_path;
    bool has_output = false;
    bool explicit_format = false;
    markup::Format format = markup::Format::Markdown;
    markup::Options options;

    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "-h" || arg == "--help") { help(); return 0; }
        if (arg == "-v" || arg == "--version") { std::cout << "Markup++ 0.1.0\n"; return 0; }
        if (arg == "--standalone") { options.standalone = true; continue; }
        if (arg == "--safe") { options.allow_raw_html = false; continue; }
        if (arg == "--commonmark") {
            options.markdown_profile = markup::Options::MarkdownProfile::CommonMark;
            continue;
        }
        if (arg == "--extensions") {
            options.markdown_profile = markup::Options::MarkdownProfile::Extended;
            continue;
        }
        if (arg == "-o" || arg == "--output") {
            if (++i >= argc) { std::cerr << "markup: " << arg << " requires a file\n"; return 2; }
            output_path = argv[i];
            has_output = true;
            continue;
        }
        if (arg == "-f" || arg == "--format") {
            if (++i >= argc) { std::cerr << "markup: " << arg << " requires a format\n"; return 2; }
            if (!parse_format(argv[i], format)) {
                std::cerr << "markup: unsupported format '" << argv[i] << "'\n";
                return 2;
            }
            explicit_format = true;
            continue;
        }
        if (arg == "--title") {
            if (++i >= argc) { std::cerr << "markup: --title requires text\n"; return 2; }
            options.title = argv[i];
            continue;
        }
        if (arg != "-" && !arg.empty() && arg[0] == '-') {
            std::cerr << "markup: unknown option '" << arg << "'\n";
            return 2;
        }
        if (!input_path.empty()) {
            std::cerr << "markup: exactly one input is supported\n";
            return 2;
        }
        input_path = arg;
    }

    if (input_path.empty()) { help(); return 1; }
    if (!explicit_format) {
        if (input_path == "-") {
            std::cerr << "markup: stdin requires --format\n";
            return 2;
        }
        if (!markup::format_for_extension(input_path.extension().string(), format)) {
            std::cerr << "markup: cannot infer a format from '" << input_path.string() << "'\n";
            return 2;
        }
    }

    std::string source;
    if (input_path == "-") {
        if (!read_stream(std::cin, source)) { std::cerr << "markup: cannot read stdin\n"; return 1; }
    } else if (!read_file(input_path, source)) {
        std::cerr << "markup: cannot read '" << input_path.string() << "'\n";
        return 1;
    }

    std::string html, error;
    if (!markup::convert(format, source, html, error, options)) {
        std::cerr << "markup: " << error << "\n";
        return 1;
    }
    if (!has_output) {
        std::cout << html;
        return std::cout ? 0 : 1;
    }
    if (output_path == input_path) {
        std::cerr << "markup: refusing to overwrite the input file\n";
        return 2;
    }
    std::string write_error;
    if (!write_file(output_path, html, write_error)) {
        std::cerr << "markup: cannot write '" << output_path.string() << "': " << write_error << "\n";
        return 1;
    }
    return 0;
}
