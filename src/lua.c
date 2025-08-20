#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
#include <stdlib.h>
#include <string.h>

#include "arr_list.h"
#include "dict.h"
#include "globals.h"

extern Dict *var_dict;

static void push_dict_to_lua(const char *key, void *value, void *user_context) {
    lua_State *L = (lua_State *)user_context;
    lua_pushstring(L, (const char *)value);
    lua_setglobal(L, key);
}

static int lua_dict_replace(lua_State *L) {
    const char *key = luaL_checkstring(L, 1);
    const char *value = luaL_checkstring(L, 2);

    dict_set(var_dict, key, strdup(value));
    return 0;
}

static int lua_dict_arr_append(lua_State *L) {
    const char *list_name = luaL_checkstring(L, 1);
    const char *value = luaL_checkstring(L, 2);

    ArrayList *list = DICT_GET_AS(ArrayList, var_dict, list_name);
    arraylist_append(list, strdup(value), true);

    return 0;
}

static int lua_dict_arr_remove(lua_State *L) {
    const char *list_name = luaL_checkstring(L, 1);
    const char *value = luaL_checkstring(L, 2);

    ArrayList *list = DICT_GET_AS(ArrayList, var_dict, list_name);
    arraylist_remove(list, (void *)value, str_cmp);
    return 0;
}

static int lua_redirect(lua_State *L) {
    const char *url = luaL_checkstring(L, 1);
    char redirect_buf[512];
    snprintf(redirect_buf, sizeof(redirect_buf), "REDIRECT:%s", url);
    lua_pushstring(L, redirect_buf);
    return 1;
}

void decode_form_string(const char *src, char *dest) {
    while (*src) {
        if (*src == '+') {
            *dest++ = ' ';
            src++;
        } else {
            *dest++ = *src++;
        }
    }
    *dest = '\0';
}

static void push_post_body_to_lua(lua_State *L, const char *body) {
    if (!body) return;

    char *copy = strdup(body);
    if (!copy) return;

    char *pair = strtok(copy, "&");
    while (pair) {
        char *eq = strchr(pair, '=');
        if (eq) {
            *eq = '\0';

            char key[128], value[512];
            decode_form_string(pair, key);
            decode_form_string(eq + 1, value);

            lua_pushstring(L, value);
            lua_setglobal(L, key);
        }

        pair = strtok(NULL, "&");
    }

    free(copy);
}

char *execute_lua(const char *lua_file, const char *post_body) {
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);

    dict_iterate(var_dict, push_dict_to_lua, L);

    if (post_body) {
        lua_pushstring(L, post_body);
        lua_setglobal(L, "POST_BODY");
        push_post_body_to_lua(L, post_body);
    }

    lua_pushcfunction(L, lua_dict_replace);
    lua_setglobal(L, "set_var");

    lua_pushcfunction(L, lua_dict_arr_append);
    lua_setglobal(L, "append_to_arr");

    lua_pushcfunction(L, lua_dict_arr_remove);
    lua_setglobal(L, "remove_from_arr");

    lua_pushcfunction(L, lua_redirect);
    lua_setglobal(L, "redirect");

    size_t code_len = strlen(lua_file);
    if (luaL_loadbuffer(L, lua_file, code_len, "line") != LUA_OK) {
        fprintf(stderr, "Lua compile error: %s\n", lua_tostring(L, -1));
        lua_close(L);
        return strdup("Internal Server Error");
    }

    if (lua_pcall(L, 0, 1, 0) != LUA_OK) {
        fprintf(stderr, "Lua runtime error: %s\n", lua_tostring(L, -1));
        lua_close(L);
        return strdup("Internal Server Error");
    }

    const char *result = lua_tostring(L, -1);
    char *ret = strdup(result ? result : "");
    lua_pop(L, 1);
    lua_close(L);

    return ret;
}
