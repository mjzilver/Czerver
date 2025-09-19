#include "template.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arr_list.h"
#include "buff.h"
#include "file.h"
#include "globals.h"
#include "utils.h"

const char *VAR_DELIM_START = "{{";
const char *VAR_DELIM_END = "}}";

const char *PARENT_DELIM_START = "%";
const char *PARENT_DELIM_END = "%";

const char *LOOP_DELIM_START = "#for";
const char *LOOP_DELIM_END = "#endfor";

const char *CHILD_CONTENT_MARKER = "_content_";

extern Dict *var_dict;

char *extract_key(const char *start, const char *end, size_t delim_len) {
    size_t key_length = end - (start + delim_len);
    char *key = malloc(key_length + 1);
    assert(key != NULL);
    strncpy(key, start + delim_len, key_length);
    key[key_length] = '\0';
    trim_whitespace(key);
    return key;
}

Buffer *parse_variables(const char *template_content) {
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
        const char *replacement = DICT_GET_AS(const char, var_dict, key);

        if (replacement) {
            buffer_append(processed_content, replacement, strlen(replacement));
        }

        free(key);
        current_pos = next_delim_end + strlen(VAR_DELIM_END);
    }

    buffer_append(processed_content, current_pos, strlen(current_pos));
    return processed_content;
}

Buffer *parse_loops(const char *template_content) {
    Buffer *processed_content = buffer_new(strlen(template_content) * 2);

    const char *current_pos = template_content;
    const char *loop_start;

    while ((loop_start = strstr(current_pos, LOOP_DELIM_START)) != NULL) {
        buffer_append(processed_content, current_pos, loop_start - current_pos);

        const char *line_end = strchr(loop_start, '\n');
        if (!line_end) {
            fprintf(stderr, "Invalid loop: missing newline after #for\n");
            return NULL;
        }

        char *header = strndup(loop_start, line_end - loop_start);
        char var_name[64], list_name[64];
        if (sscanf(header, "#for %63s in %63s", var_name, list_name) != 2) {
            fprintf(stderr, "Invalid loop syntax: %s\n", header);
            free(header);
            return NULL;
        }
        free(header);

        const char *loop_end = strstr(line_end, LOOP_DELIM_END);
        if (!loop_end) {
            fprintf(stderr, "Unmatched loop: missing #endfor\n");
            return NULL;
        }

        const char *body_start = line_end + 1;
        size_t body_len = loop_end - body_start;
        char *body = strndup(body_start, body_len);

        ArrayList *list = DICT_GET_AS(ArrayList, var_dict, list_name);
        if (!list) {
            fprintf(stderr, "List not found: %s\n", list_name);
            free(body);
            return NULL;
        }

        for (size_t i = 0; i < list->len; i++) {
            dict_set(var_dict, var_name, list->items[i].value);

            Buffer *expanded = parse_variables(body);
            buffer_append(processed_content, expanded->data, expanded->length);
            buffer_free(expanded);
        }

        free(body);

        current_pos = loop_end + strlen(LOOP_DELIM_END);
    }

    buffer_append(processed_content, current_pos, strlen(current_pos));
    return processed_content;
}

Buffer *parse_parent_template(const char *template_content) {
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

char *process_template(const char *template_content) {
    char *content = strdup(template_content);
    while (strstr(content, PARENT_DELIM_START) != NULL) {
        Buffer *combined_content = parse_parent_template(content);
        free(content);
        content = buffer_take_data(combined_content);
    }

    Buffer *loop_content = parse_loops(content);
    free(content);
    content = buffer_take_data(loop_content);

    Buffer *final_content = parse_variables(content);
    free(content);

    return buffer_take_data(final_content);
}
