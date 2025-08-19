#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
#include <stdlib.h>
#include <string.h>

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

static int lua_redirect(lua_State *L) {
    const char *url = luaL_checkstring(L, 1);
    char redirect_buf[512];
    snprintf(redirect_buf, sizeof(redirect_buf), "REDIRECT:%s", url);
    lua_pushstring(L, redirect_buf);
    return 1;
}

// TODO: Add URL string decoder
static void push_post_body_to_lua(lua_State *L, const char *body) {
    if (!body) return;

    char *body_copy = strdup(body);
    if (!body_copy) return;

    char *pair = strtok(body_copy, "&");
    while (pair) {
        char key[64], value[256];
        if (sscanf(pair, "%63[^=]=%255s", key, value) == 2) {
            lua_pushstring(L, value);
            lua_setglobal(L, key);
        }
        pair = strtok(NULL, "&");
    }

    free(body_copy);
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
    lua_setglobal(L, "dict_replace");

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
