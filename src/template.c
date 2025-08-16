#include "template.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "file.h"
#include "utils.h"
#include "buff.h"

const char *VAR_DELIM_START = "{{";
const char *VAR_DELIM_END = "}}";

const char *PARENT_DELIM_START = "%";
const char *PARENT_DELIM_END = "%";

const char *CHILD_CONTENT_MARKER = "_content_";

char *extract_key(const char *start, const char *end, size_t delim_len) {
    size_t key_length = end - (start + delim_len);
    char *key = malloc(key_length + 1);
    assert(key != NULL);
    strncpy(key, start + delim_len, key_length);
    key[key_length] = '\0';
    trim_whitespace(key);
    return key;
}

Buffer *parse_variables(const char *template_content, KeyValuePair *replacements) {
    Buffer *processed_content = buffer_new(strlen(template_content) * 2);

    const char *current_pos = template_content;
    const char *next_delim_start;

    while ((next_delim_start = strstr(current_pos, VAR_DELIM_START)) != NULL) {
        buffer_append(processed_content, current_pos, next_delim_start - current_pos);

        const char *next_delim_end = strstr(next_delim_start + strlen(VAR_DELIM_START), VAR_DELIM_END);
        if (!next_delim_end) {
            fprintf(stderr, "Unmatched delimiter: %s\n", VAR_DELIM_START);
            return NULL;
        }

        char *key = extract_key(next_delim_start, next_delim_end, strlen(VAR_DELIM_START));

        const char *replacement = NULL;
        for (size_t i = 0; replacements[i].key != NULL; i++) {
            if (strcmp(key, replacements[i].key) == 0) {
                replacement = replacements[i].value;
                break;
            }
        }

        if (replacement) {
            buffer_append(processed_content, replacement, strlen(replacement));
        }

        free(key);
        current_pos = next_delim_end + strlen(VAR_DELIM_END);
    }

    buffer_append(processed_content, current_pos, strlen(current_pos));
    return processed_content;
}

Buffer *parse_parent_template(const char *template_content, const char *main_content) {
    Buffer *processed_content = buffer_new(strlen(template_content) * 2);

    const char *current_pos = template_content;
    const char *next_delim_start;

    while ((next_delim_start = strstr(current_pos, PARENT_DELIM_START)) != NULL) {
        buffer_append(processed_content, current_pos, next_delim_start - current_pos);

        const char *next_delim_end = strstr(next_delim_start + strlen(PARENT_DELIM_START), PARENT_DELIM_END);
        if (!next_delim_end) {
            fprintf(stderr, "Unmatched delimiter: %s\n", PARENT_DELIM_START);
            return NULL;
        }

        char *key = extract_key(next_delim_start, next_delim_end, strlen(PARENT_DELIM_START));

        char *included_content = read_file(key);
        if (!included_content) {
            fprintf(stderr, "Failed to read included file: %s\n", key);
            free(key);
            return NULL;
        }

        const char *placeholder_pos = strstr(included_content, CHILD_CONTENT_MARKER);
        if (placeholder_pos) {
            buffer_append(processed_content, included_content, placeholder_pos - included_content);
        } else {
            fprintf(stderr, "Failed to find placeholder in included file: %s\n", key);
            buffer_append(processed_content, included_content, strlen(included_content));
        }

        free(included_content);
        free(key);
        current_pos = next_delim_end + strlen(PARENT_DELIM_END);
    }

    buffer_append(processed_content, current_pos, strlen(current_pos));
    return processed_content;
}

char *process_template(const char *template_content, KeyValuePair *replacements) {
    char *content = strdup(template_content);
    while (strstr(content, PARENT_DELIM_START) != NULL) {
        Buffer *combined_content = parse_parent_template(content, template_content);
        free(content);
        content = buffer_take_data(combined_content);
    }

    Buffer *final_content = parse_variables(content, replacements);
    free(content);

    return buffer_take_data(final_content);
}
