#include <ctype.h>
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
#include <stdlib.h>
#include <string.h>

#include "arr_list.h"
#include "arr_list_utils.h"
#include "dict.h"
#include "globals.h"
#include "string_utils.h"

extern Dict *var_dict;

static void push_dict_to_lua(const char *key, void *value, void *user_context) {
    lua_State *lua = (lua_State *)user_context;
    lua_pushstring(lua, (const char *)value);
    lua_setglobal(lua, key);
}

static int lua_dict_replace(lua_State *lua) {
    const char *key = luaL_checkstring(lua, 1);
    const char *value = luaL_checkstring(lua, 2);

    dict_set(var_dict, key, strdup(value));
    return 0;
}

static int lua_dict_arr_append(lua_State *lua) {
    const char *list_name = luaL_checkstring(lua, 1);
    const char *value = luaL_checkstring(lua, 2);

    ArrayList *list = DICT_GET_AS(ArrayList, var_dict, list_name);
    if (!list) {
        list = arraylist_new(10);
        dict_set(var_dict, list_name, list);
    }

    arraylist_append(list, strdup(value), true);

    return 0;
}

static int lua_dict_get_string(lua_State *lua) {
    const char *key = luaL_checkstring(lua, 1);

    char *str = DICT_GET_AS(char, var_dict, key);
    if (str) {
        lua_pushstring(lua, str);
    } else {
        lua_pushnil(lua);
    }

    return 1;
}

static int lua_dict_get_array(lua_State *lua) {
    const char *key = luaL_checkstring(lua, 1);
    ArrayList *list = DICT_GET_AS(ArrayList, var_dict, key);

    if (!list) {
        lua_pushnil(lua);
        return 1;
    }

    lua_newtable(lua);
    for (int i = 0; i < list->len; i++) {
        lua_pushinteger(lua, i + 1);
        lua_pushstring(lua, (char *)list->items[i].value);
        lua_settable(lua, -3);
    }

    return 1;
}

static int lua_dict_arr_remove(lua_State *lua) {
    const char *list_name = luaL_checkstring(lua, 1);
    const char *value = luaL_checkstring(lua, 2);

    ArrayList *list = DICT_GET_AS(ArrayList, var_dict, list_name);
    arraylist_remove(list, (void *)value, string_compare_function);
    return 0;
}

static int lua_redirect(lua_State *lua) {
    const char *url = luaL_checkstring(lua, 1);
    char redirect_buf[512];
    snprintf(redirect_buf, sizeof(redirect_buf), "REDIRECT:%s", url);
    lua_pushstring(lua, redirect_buf);
    return 1;
}

void decode_form_string(const char *src, char *dest) {
    while (*src) {
        if (*src == '+') {
            *dest++ = ' ';
            src++;
        } else if (*src == '%' && isxdigit(src[1]) && isxdigit(src[2])) {
            int hex;
            sscanf(src + 1, "%2x", &hex);
            *dest++ = (char)hex;
            src += 3;
        } else {
            *dest++ = *src++;
        }
    }
    *dest = '\0';
}

static void push_post_body_to_lua(lua_State *lua, const char *body) {
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

            lua_pushstring(lua, value);
            lua_setglobal(lua, key);
        }

        pair = strtok(NULL, "&");
    }

    free(copy);
}

char *execute_lua(const char *lua_file, const char *post_body) {
    lua_State *lua = luaL_newstate();
    luaL_openlibs(lua);

    dict_iterate(var_dict, push_dict_to_lua, lua);

    if (post_body) {
        lua_pushstring(lua, post_body);
        lua_setglobal(lua, "POST_BODY");
        push_post_body_to_lua(lua, post_body);
    }

    lua_pushcfunction(lua, lua_dict_replace);
    lua_setglobal(lua, "set_var");

    lua_pushcfunction(lua, lua_dict_get_string);
    lua_setglobal(lua, "get_var");

    lua_pushcfunction(lua, lua_dict_get_array);
    lua_setglobal(lua, "get_arr");

    lua_pushcfunction(lua, lua_dict_arr_append);
    lua_setglobal(lua, "append_to_arr");

    lua_pushcfunction(lua, lua_dict_arr_remove);
    lua_setglobal(lua, "remove_from_arr");

    lua_pushcfunction(lua, lua_redirect);
    lua_setglobal(lua, "redirect");

    size_t code_len = strlen(lua_file);
    if (luaL_loadbuffer(lua, lua_file, code_len, "line") != LUA_OK) {
        fprintf(stderr, "Lua compile error: %s\n", lua_tostring(lua, -1));
        lua_close(lua);
        return strdup("Internal Server Error");
    }

    if (lua_pcall(lua, 0, 1, 0) != LUA_OK) {
        fprintf(stderr, "Lua runtime error: %s\n", lua_tostring(lua, -1));
        lua_close(lua);
        return strdup("Internal Server Error");
    }

    const char *result = lua_tostring(lua, -1);
    char *ret = strdup(result ? result : "");
    lua_pop(lua, 1);
    lua_close(lua);

    return ret;
}
