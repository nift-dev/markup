#include "cmark.h"

#include <stdio.h>
#include <stdlib.h>

int main(void) {
    size_t length = 0;
    size_t capacity = 4096;
    char *input = malloc(capacity);
    if (!input) return 2;
    for (;;) {
        if (length == capacity) {
            capacity *= 2;
            char *grown = realloc(input, capacity);
            if (!grown) { free(input); return 2; }
            input = grown;
        }
        const size_t got = fread(input + length, 1, capacity - length, stdin);
        length += got;
        if (got == 0) break;
    }
    if (ferror(stdin)) { free(input); return 2; }

    char *html = cmark_markdown_to_html(
        input, length, CMARK_OPT_VALIDATE_UTF8 | CMARK_OPT_UNSAFE);
    free(input);
    if (!html) return 2;
    const int failed = fputs(html, stdout) == EOF;
    cmark_get_default_mem_allocator()->free(html);
    return failed ? 2 : 0;
}
