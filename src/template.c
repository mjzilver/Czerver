#include "template.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

const char *delim_start = "{{";
const char *delim_end = "}}";

void resize_if_needed(char **final_content, size_t *final_length, size_t needed_length) {
    if (*final_length < needed_length) {
        *final_length = needed_length * 2;
        *final_content = realloc(*final_content, *final_length);
        assert(*final_content != NULL);
    }
}

char *replace_variables(const char *html_content, KeyValuePair replacements[], size_t count) {
    size_t final_length = strlen(html_content) * 2;
    char *final_content = malloc(final_length);
    assert(final_content != NULL);
    final_content[0] = '\0';

    const char *current_pos = html_content;
    char *final_ptr = final_content;

    // Iterate through the HTML content
    while ((current_pos = strstr(current_pos, delim_start)) != NULL) {
        // Copy everything before the delimiter into final_content
        size_t part_length = current_pos - html_content;
        resize_if_needed(&final_content, &final_length, strlen(final_content) + part_length + 1);
        strncpy(final_ptr, html_content, part_length);
        final_ptr += part_length;

        // Skip the start delimiter
        current_pos += strlen(delim_start);

        // Find the end delimiter
        const char *end_pos = strstr(current_pos, delim_end);
        if (end_pos == NULL) {
            break;
        }

        // Extract the key
        size_t key_length = end_pos - current_pos;
        char *key = malloc(key_length + 1);
        strncpy(key, current_pos, key_length);
        key[key_length] = '\0';
        trim_whitespace(key);

        // Find the value corresponding to the key
        const char *value = NULL;
        for (size_t i = 0; i < count; i++) {
            if (strcmp(key, replacements[i].key) == 0) {
                value = replacements[i].value;
                break;
            }
        }
        free(key);

        // If a value is found, append it to the final content
        if (value != NULL) {
            size_t value_len = strlen(value);
            resize_if_needed(&final_content, &final_length, strlen(final_content) + value_len + 1);
            strcpy(final_ptr, value);
            final_ptr += value_len;
        }

        // Move past the end delimiter
        current_pos = end_pos + strlen(delim_end);
        html_content = current_pos;
    }

    // Append any remaining content after the last delimiter
    if (html_content != NULL) {
        size_t remaining_length = strlen(html_content);
        resize_if_needed(&final_content, &final_length, strlen(final_content) + remaining_length + 1);
        strcpy(final_ptr, html_content);
    }

    return final_content;
}
