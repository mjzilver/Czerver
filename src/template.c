#include "template.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "file.h"

const char *delim_start = "{{";
const char *delim_end = "}}";

const char *parent_delim_start = "%";
const char *parent_delim_end = "%";

const char *child_content_marker = "_content_";

void resize_if_needed(char **buffer, size_t *buffer_size, size_t needed_size) {
    if (*buffer_size < needed_size) {
        *buffer_size = needed_size * 2;
        *buffer = realloc(*buffer, *buffer_size);
        assert(*buffer != NULL);
    }
}

void append_to_content(char **buffer, size_t *buffer_size, const char *content, size_t length) {
    size_t current_length = strlen(*buffer);
    resize_if_needed(buffer, buffer_size, current_length + length + 1);
    strncat(*buffer, content, length);
}

char *parse_variables(const char *html_content, KeyValuePair *replacements) {
    size_t buffer_size = strlen(html_content) * 2;
    char *result = malloc(buffer_size);
    assert(result != NULL);
    result[0] = '\0';

    const char *current_pos = html_content;
    const char *next_delim_start;

    while ((next_delim_start = strstr(current_pos, delim_start)) != NULL) {
        // Append content before the start delimiter
        append_to_content(&result, &buffer_size, current_pos, next_delim_start - current_pos);

        // Find the end delimiter
        const char *next_delim_end = strstr(next_delim_start + strlen(delim_start), delim_end);
        if (!next_delim_end) {
            fprintf(stderr, "Unmatched delimiter: %s\n", delim_start);
            break;
        }

        // Extract the key inside delimiters
        size_t key_length = next_delim_end - (next_delim_start + strlen(delim_start));
        char *key = malloc(key_length + 1);
        assert(key != NULL);
        strncpy(key, next_delim_start + strlen(delim_start), key_length);
        key[key_length] = '\0';
        trim_whitespace(key);

        // Replace the key with the corresponding value
        const char *replacement = NULL;
        for (size_t i = 0; replacements[i].key != NULL; i++) {
            if (strcmp(key, replacements[i].key) == 0) {
                replacement = replacements[i].value;
                break;
            }
        }

        if (replacement) {
            append_to_content(&result, &buffer_size, replacement, strlen(replacement));
        }

        free(key);
        current_pos = next_delim_end + strlen(delim_end);
    }

    // Append remaining content
    append_to_content(&result, &buffer_size, current_pos, strlen(current_pos));
    return result;
}

char *parse_parent_template(const char *html_content, const char *main_content) {
    size_t buffer_size = strlen(html_content) * 2;
    char *result = malloc(buffer_size);
    assert(result != NULL);
    result[0] = '\0';

    const char *current_pos = html_content;
    const char *next_delim_start;

    while ((next_delim_start = strstr(current_pos, parent_delim_start)) != NULL) {
        // Append content before the start delimiter
        append_to_content(&result, &buffer_size, current_pos, next_delim_start - current_pos);

        // Find the end delimiter
        const char *next_delim_end = strstr(next_delim_start + strlen(parent_delim_start), parent_delim_end);
        if (!next_delim_end) {
            fprintf(stderr, "Unmatched delimiter: %s\n", parent_delim_start);
            break;
        }

        // Extract the file name inside delimiters
        size_t key_length = next_delim_end - (next_delim_start + strlen(parent_delim_start));
        char *key = malloc(key_length + 1);
        assert(key != NULL);
        strncpy(key, next_delim_start + strlen(parent_delim_start), key_length);
        key[key_length] = '\0';
        trim_whitespace(key);

        // Read the included template file
        char *included_content = read_file(key);
        if (!included_content) {
            fprintf(stderr, "Failed to read included file: %s\n", key);
            free(key);
            break;
        }

        // Insert the main content into the included template
        const char *placeholder_pos = strstr(included_content, child_content_marker);
        if (placeholder_pos) {
            append_to_content(&result, &buffer_size, included_content, placeholder_pos - included_content);
        } else {
            fprintf(stderr, "Failed to find placeholder in included file: %s\n", key);
            append_to_content(&result, &buffer_size, included_content, strlen(included_content));
        }

        free(included_content);
        free(key);
        current_pos = next_delim_end + strlen(parent_delim_end);
    }

    // Append remaining content
    append_to_content(&result, &buffer_size, current_pos, strlen(current_pos));
    return result;
}

char *process_template(const char *html_content, KeyValuePair *replacements) {
    // Handle parent templates first
    char *content_with_includes = parse_parent_template(html_content, html_content);

    // Replace variables
    char *final_content = parse_variables(content_with_includes, replacements);
    free(content_with_includes);

    return final_content;
}
