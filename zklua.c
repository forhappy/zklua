/* Zklua: Lua Binding Of Apache ZooKeeper
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef WIN32
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#include "zklua.h"

static FILE *zklua_log_stream = NULL;

/**
 * unique static address used for indexing watcher_fn
 * in LUA_REGISTRYINDEX.
 **/
static char watcher_fn_key = 'k';

/**
 * unique static address used for indexing zklua_handle_t zh
 * in LUA_REGISTRYINDEX.
 **/
static char zklua_handle_key = 'k';

static int _zklua_build_stat(lua_State *L, const struct Stat *stat);

static int _zklua_build_string_vector(lua_State *L, const struct String_vector *sv);

static int _zklua_build_acls(lua_State *L, const struct ACL_vector *acls);

static int _zklua_unref(lua_State *L, int ref);

void watcher_dispatch(zhandle_t *zh, int type, int state,
        const char *path, void *watcherctx)
{
    zklua_global_watcher_context_t *wrapper = (zklua_global_watcher_context_t *)watcherctx;
    lua_State *L = wrapper->L;
    void *context = wrapper->context;

    /** push lua watcher_fn onto the stack. */
    lua_pushlightuserdata(L, (void *)&watcher_fn_key);
    lua_gettable(L, LUA_REGISTRYINDEX);
    /* push zklua_handle_t onto the stack. */
    lua_pushlightuserdata(L, (void *)&zklua_handle_key);
    lua_gettable(L, LUA_REGISTRYINDEX);
    /** push type onto the stack. */
    lua_pushinteger(L, type);
    /** push state onto the stack. */
    lua_pushinteger(L, state);
    /** push path onto the stack. */
    lua_pushstring(L, path);
    /** push watcher context onto the stack. */
    lua_pushstring(L, context);
    lua_call(L, 5, 0);
}

void local_watcher_dispatch(zhandle_t *zh, int type, int state,
        const char *path, void *watcherctx)
{
    zklua_local_watcher_context_t *wrapper = (zklua_local_watcher_context_t *)watcherctx;
    lua_State *L = wrapper->L;
    void *context = wrapper->context;
    int zhref = wrapper->zhref;
    int cbref = wrapper->cbref;

    /** push lua watcher_fn onto the stack. */
    lua_rawgeti(L, LUA_REGISTRYINDEX, cbref);
    /* push zklua_handle_t onto the stack. */
    lua_rawgeti(L, LUA_REGISTRYINDEX, zhref);
    /** push type onto the stack. */
    lua_pushinteger(L, type);
    /** push state onto the stack. */
    lua_pushinteger(L, state);
    /** push path onto the stack. */
    lua_pushstring(L, path);
    /** push watcher context onto the stack. */
    lua_pushstring(L, context);
    lua_call(L, 5, 0);
    _zklua_unref(L, zhref);
    _zklua_unref(L, cbref);
}

void void_completion_dispatch(int rc, const void *data)
{
    zklua_completion_data_t *wrapper = (zklua_completion_data_t *)data;
    lua_State *L = wrapper->L;
    const char *real_data = wrapper->data;
    lua_pushinteger(L, rc);
    lua_pushstring(L, real_data);
    lua_call(L, 2, 0);
    free(wrapper);
}

void stat_completion_dispatch(int rc, const struct Stat *stat,
        const void *data)
{
    zklua_completion_data_t *wrapper = (zklua_completion_data_t *)data;
    lua_State *L = wrapper->L;
    const char *real_data = wrapper->data;
    lua_pushinteger(L, rc);
    _zklua_build_stat(L, stat);
    lua_pushstring(L, real_data);
    lua_call(L, 3, 0);
    free(wrapper);
}

void data_completion_dispatch(int rc, const char *value, int value_len,
        const struct Stat *stat, const void *data)
{
    zklua_completion_data_t *wrapper = (zklua_completion_data_t *)data;
    lua_State *L = wrapper->L;
    const char *real_data = wrapper->data;
    lua_pushinteger(L, rc);
    lua_pushlstring(L, value, value_len);
    _zklua_build_stat(L, stat);
    lua_pushstring(L, real_data);
    lua_call(L, 4, 0);
    free(wrapper);
}

void strings_completion_dispatch(int rc, const struct String_vector *strings,
        const void *data)
{
    zklua_completion_data_t *wrapper = (zklua_completion_data_t *)data;
    lua_State *L = wrapper->L;
    const char *real_data = wrapper->data;
    lua_pushinteger(L, rc);
    _zklua_build_string_vector(L, strings);
    lua_pushstring(L, real_data);
    lua_call(L, 3, 0);
    free(wrapper);
}

void strings_stat_completion_dispatch(int rc, const struct String_vector *strings,
        const struct Stat *stat, const void *data)
{
    zklua_completion_data_t *wrapper = (zklua_completion_data_t *)data;
    lua_State *L = wrapper->L;
    const char *real_data = wrapper->data;
    lua_pushinteger(L, rc);
    _zklua_build_string_vector(L, strings);
    _zklua_build_stat(L, stat);
    lua_pushstring(L, real_data);
    lua_call(L, 4, 0);
    free(wrapper);

}

void string_completion_dispatch(int rc, const char *value, const void *data)
{
    zklua_completion_data_t *wrapper = (zklua_completion_data_t *)data;
    lua_State *L = wrapper->L;
    const char *real_data = wrapper->data;
    lua_pushinteger(L, rc);
    lua_pushstring(L, value);
    lua_pushstring(L, real_data);
    lua_call(L, 3, 0);
    free(wrapper);
}

void acl_completion_dispatch(int rc, struct ACL_vector *acl,
        struct Stat *stat, const void *data)
{
    zklua_completion_data_t *wrapper = (zklua_completion_data_t *)data;
    lua_State *L = wrapper->L;
    const char *real_data = wrapper->data;
    lua_pushinteger(L, rc);
    _zklua_build_acls(L, acl);
    _zklua_build_stat(L, stat);
    lua_pushstring(L, real_data);
    lua_call(L, 3, 0);
    free(wrapper);
}

/**
 * return 1 if @host@ has the following format:
 * "127.0.0.1:2081,127.0.0.1:2082,127.0.0.1:2083".
 * TODO: check if the given host is valid.
 **/
static int _zklua_check_host(const char *host)
{
    return 1;
}

static int _zklua_build_stat(lua_State *L, const struct Stat *stat)
{
    if (stat != NULL) {
        lua_newtable(L);
        lua_pushstring(L, "czxid");
        lua_pushnumber(L, stat->czxid);
        lua_settable(L, -3);
        lua_pushstring(L, "mzxid");
        lua_pushnumber(L, stat->mzxid);
        lua_settable(L, -3);
        lua_pushstring(L, "ctime");
        lua_pushnumber(L, stat->ctime);
        lua_settable(L, -3);
        lua_pushstring(L, "mtime");
        lua_pushnumber(L, stat->mtime);
        lua_settable(L, -3);
        lua_pushstring(L, "version");
        lua_pushnumber(L, stat->version);
        lua_settable(L, -3);
        lua_pushstring(L, "cversion");
        lua_pushnumber(L, stat->cversion);
        lua_settable(L, -3);
        lua_pushstring(L, "aversion");
        lua_pushnumber(L, stat->aversion);
        lua_settable(L, -3);
        lua_pushstring(L, "ephemeralOwner");
        lua_pushnumber(L, stat->ephemeralOwner);
        lua_settable(L, -3);
        lua_pushstring(L, "dataLength");
        lua_pushnumber(L, stat->dataLength);
        lua_settable(L, -3);
        lua_pushstring(L, "numChildren");
        lua_pushnumber(L, stat->numChildren);
        lua_settable(L, -3);
        lua_pushstring(L, "pzxid");
        lua_pushnumber(L, stat->pzxid);
        lua_settable(L, -3);
    } else {
        lua_newtable(L);
        lua_pushstring(L, "czxid");
        lua_pushnumber(L, 0);
        lua_settable(L, -3);
        lua_pushstring(L, "mzxid");
        lua_pushnumber(L, 0);
        lua_settable(L, -3);
        lua_pushstring(L, "ctime");
        lua_pushnumber(L, 0);
        lua_settable(L, -3);
        lua_pushstring(L, "mtime");
        lua_pushnumber(L, 0);
        lua_settable(L, -3);
        lua_pushstring(L, "version");
        lua_pushnumber(L, 0);
        lua_settable(L, -3);
        lua_pushstring(L, "cversion");
        lua_pushnumber(L, 0);
        lua_settable(L, -3);
        lua_pushstring(L, "aversion");
        lua_pushnumber(L, 0);
        lua_settable(L, -3);
        lua_pushstring(L, "ephemeralOwner");
        lua_pushnumber(L, 0);
        lua_settable(L, -3);
        lua_pushstring(L, "dataLength");
        lua_pushnumber(L, 0);
        lua_settable(L, -3);
        lua_pushstring(L, "numChildren");
        lua_pushnumber(L, 0);
        lua_settable(L, -3);
        lua_pushstring(L, "pzxid");
        lua_pushnumber(L, 0);
        lua_settable(L, -3);
    }
    return 1;
}

static int _zklua_build_string_vector(lua_State *L, const struct String_vector *sv)
{
    int i;
    lua_newtable(L);
    if (sv != NULL) {
        for (i = 0; i < sv->count; ++i) {
            lua_pushstring(L, sv->data[i]);
            lua_rawseti(L, -2, i);
        }
    }
    return 0;
}

static int _zklua_build_acls(lua_State *L, const struct ACL_vector *acls)
{
    int i;
    lua_newtable(L);
    if (acls != NULL) {
        for (i = 0; i < acls->count; ++i) {
            lua_newtable(L);
            lua_pushstring(L, "perms");
            lua_pushnumber(L, acls->data[i].perms);
            lua_settable(L, -3);
            lua_pushstring(L, "scheme");
            lua_pushstring(L, acls->data[i].id.scheme);
            lua_settable(L, -3);
            lua_pushstring(L, "id");
            lua_pushstring(L, acls->data[i].id.id);
            lua_settable(L, -3);
            lua_rawseti(L, -2, i+1);
        }
    }
    return 0;
}

/**
 * parse an ACL_vector struct from the given acceptable index.
 * return 1 if the parsing is successful, otherwise 0 returned.
 **/
static int _zklua_parse_acls(lua_State *L, int index, struct ACL_vector *acls)
{
    int count = 0, i = 0, j = 0;
    luaL_checktype(L, index, LUA_TTABLE);
    count = lua_objlen(L, index);
    acls->count = count;
    acls->data = (struct ACL *)calloc(count, sizeof(struct ACL));
    if (acls->data == NULL) {
        luaL_error(L, "out of memory when zklua trys to "
                "alloc an internal object.");
        return 0;
    }
    for (i = 1; i <= count; i++) {
        lua_rawgeti(L, index, i);

        lua_pushstring(L, "perms");
        lua_rawget(L, -2);
        acls->data[i-1].perms = lua_tointeger(L, -1);
        lua_pop(L, 1);

        lua_pushstring(L, "scheme");
        lua_rawget(L, -2);
        acls->data[i-1].id.scheme = strdup(lua_tostring(L, -1));
        lua_pop(L, 1);

        lua_pushstring(L, "id");
        lua_rawget(L, -2);
        acls->data[i-1].id.id = strdup(lua_tostring(L, -1));
        lua_pop(L, 1);

        lua_pop(L, 1);
    }
    return 1;
}

/**
 * frees an ACL_vector struct.
 **/
static int _zklua_free_acls(struct ACL_vector *acls)
{
    int i;
    if (acls == NULL) {
        return -1;
    }
    for (i = 0; i < acls->count; ++i) {
        free(acls->data[i].id.id);
        free(acls->data[i].id.scheme);
    }
    free(acls->data);
    return 1;
}

static int _zklua_check_handle(lua_State *L, zklua_handle_t *handle)
{
    if (handle->zh) {
        return 1;
    } else {
        luaL_error(L, "invalid zookeeper handle.");
        return 0;
    }
}

static int _zklua_ref(lua_State *L, int index)
{
    lua_pushvalue(L, index);
    int ref = luaL_ref(L, LUA_REGISTRYINDEX);
    return ref;
}

static int _zklua_unref(lua_State *L, int ref)
{
    luaL_unref(L, LUA_REGISTRYINDEX, ref);
    return 0;
}

static zklua_global_watcher_context_t *_zklua_global_watcher_context_init(
        lua_State *L, void *data)
{
    zklua_global_watcher_context_t *wrapper = (zklua_global_watcher_context_t *)malloc(
        sizeof(zklua_global_watcher_context_t));
    if (wrapper == NULL) {
        luaL_error(L, "out of memory when zklua trys to "
                "alloc an internal object.");
    }
    wrapper->L = L;
    wrapper->context = data;
    return wrapper;
}

static zklua_local_watcher_context_t *_zklua_local_watcher_context_init(
        lua_State *L, void *data, int zhref, int cbref)
{
    zklua_local_watcher_context_t *wrapper = (zklua_local_watcher_context_t *)malloc(
        sizeof(zklua_local_watcher_context_t));
    if (wrapper == NULL) {
        luaL_error(L, "out of memory when zklua trys to "
                "alloc an internal object.");
    }
    wrapper->L = L;
    wrapper->context = data;
    wrapper->zhref = zhref;
    wrapper->cbref = cbref;
    return wrapper;
}

/**
 * save watcher_fn into LUA_REGISTRYINDEX, and @index@ is the index in lua_State
 * where the lua watcher_fn resides.
 **/
static void _zklua_save_watcherfn(lua_State *L, int index)
{
    lua_pushlightuserdata(L, (void *)&watcher_fn_key);
    lua_pushvalue(L, index);
    lua_settable(L, LUA_REGISTRYINDEX);
}

/**
 * save zklua_handle_t *zh into LUA_REGISTRYINDEX, and @index@ is
 * the index in lua_State where the lua watcher_fn resides.
 **/
static void _zklua_save_zklua_handle(lua_State *L, int index)
{
    lua_pushlightuserdata(L, (void *)&zklua_handle_key);
    lua_pushvalue(L, index);
    lua_settable(L, LUA_REGISTRYINDEX);
}

/**
 * remove zklua_handle_t *zh from LUA_REGISTRYINDEX.
 **/
static void _zklua_remove_zklua_handle(lua_State *L)
{
    lua_pushlightuserdata(L, (void *)&zklua_handle_key);
    lua_pushnil(L);
    lua_settable(L, LUA_REGISTRYINDEX);
}

/**
 * initialize C clientid_t struct from lua table.
 **/
static clientid_t *_zklua_clientid_init(
        lua_State *L, int index)
{
    size_t passwd_len = 0;
    const char *clientid_passwd = NULL;
    clientid_t *clientid = NULL;
    clientid = (clientid_t *)malloc(sizeof(clientid_t));
    if (clientid == NULL) {
        luaL_error(L, "out of memory when zklua trys to "
                "alloc an internal object.");
    }

    luaL_checktype(L, index, LUA_TTABLE);
    lua_getfield(L, index, "client_id");
    clientid->client_id =  luaL_checkint(L, -1);
    lua_pop(L, 1);
    lua_getfield(L, index, "passwd");
    clientid_passwd = luaL_checklstring(L, -1, &passwd_len);
    memset(clientid->passwd, 0, 16);
    memcpy(clientid->passwd, clientid_passwd, passwd_len);
    lua_pop(L, 1);

    return  clientid;
}

static void _zklua_clientid_fini(clientid_t **clientid)
{
    if (*clientid != NULL) {
        free(*clientid);
        *clientid = NULL;
    }
}

/**
 * initialize a zookeeper handle.
 **/
static int zklua_init(lua_State *L)
{
    int top = lua_gettop(L);
    size_t host_len = 0;
    const char *host = NULL;
    int recv_timeout = 0;
    clientid_t *clientid = NULL;
    size_t real_context_len = 0;
    char *real_watcher_context = NULL;
    int flags = 0;
    zklua_global_watcher_context_t *wrapper = NULL;

    zklua_handle_t *handle = (zklua_handle_t *)lua_newuserdata(L,
            sizeof(zklua_handle_t));
    luaL_getmetatable(L, ZKLUA_METATABLE_NAME);
    lua_setmetatable(L, -2);
    _zklua_save_zklua_handle(L, -1);

    host = luaL_checklstring(L, 1, &host_len);
    if (!_zklua_check_host(host)) {
        return luaL_error(L, "invalid arguments:"
                "host must be a string with format:\n"
                "127.0.0.1:2081,127.0.0.1:2082");
    }
    luaL_checktype(L, 2, LUA_TFUNCTION);
    _zklua_save_watcherfn(L, 2);
    recv_timeout = luaL_checkint(L, 3);
    switch(top) {
        case 3:
            wrapper = _zklua_global_watcher_context_init(L, NULL);
            handle->zh = zookeeper_init(host, watcher_dispatch,
                    recv_timeout, 0, wrapper, 0);
            break;
        case 4:
            luaL_checktype(L, 4, LUA_TTABLE);
            clientid = _zklua_clientid_init(L, 4);
            wrapper = _zklua_global_watcher_context_init(L, NULL);
            handle->zh = zookeeper_init(host, watcher_dispatch,
                    recv_timeout, clientid, wrapper, 0);
            _zklua_clientid_fini(&clientid);
            break;
        case 5:
            luaL_checktype(L, 4, LUA_TTABLE);
            clientid = _zklua_clientid_init(L, 4);
            real_watcher_context = (char *)luaL_checklstring(L, 5, &real_context_len);
            wrapper = _zklua_global_watcher_context_init(L, real_watcher_context);
            handle->zh = zookeeper_init(host, watcher_dispatch,
                    recv_timeout, clientid, wrapper, 0);
            _zklua_clientid_fini(&clientid);
            break;
        case 6:
            luaL_checktype(L, 4, LUA_TTABLE);
            clientid = _zklua_clientid_init(L, 4);
            real_watcher_context = (char *)luaL_checklstring(L, 5, &real_context_len);
            wrapper = _zklua_global_watcher_context_init(L, real_watcher_context);
            flags = luaL_checkint(L, 6);
            handle->zh = zookeeper_init(host, watcher_dispatch,
                    recv_timeout, clientid, wrapper, flags);
            _zklua_clientid_fini(&clientid);
            break;
        default:
            break;
    }
    return 1;
}

static int zklua_close(lua_State *L)
{
    int ret = 0;
    zklua_handle_t *handle = luaL_checkudata(L, 1, ZKLUA_METATABLE_NAME);
    if (handle->zh != NULL) {
        /* close zookeeper handle. */
        ret = zookeeper_close(handle->zh);
        handle->zh = NULL;
        /* remove zookeeper handle from LUA_REGISTRYINDEX. */
        _zklua_remove_zklua_handle(L);
    } else {
        return luaL_error(L, "unable to close the zookeeper handle.");
    }
    /* close log stream. */
    if (zklua_log_stream != NULL) fclose(zklua_log_stream);
    /* push ret code of zookeeper_close() onto stack. */
    lua_pushinteger(L, ret);
    return 1;
}

/**
 * return clientid_t of the current connection.
 **/
static int zklua_client_id(lua_State *L)
{
    zklua_handle_t *handle = luaL_checkudata(L, 1, ZKLUA_METATABLE_NAME);
    if (_zklua_check_handle(L, handle)) {
        const clientid_t *clientid = zoo_client_id(handle->zh);
        lua_newtable(L);
        lua_pushstring(L, "client_id");
        lua_pushnumber(L, clientid->client_id);
        lua_settable(L, -3);
        lua_pushstring(L, "passwd");
        lua_pushstring(L, clientid->passwd);
        lua_settable(L, -3);
        return 1;
    } else {
        return luaL_error(L, "unable to get client id.");
    }
}

static int zklua_recv_timeout(lua_State *L)
{
    int recv_timeout = 0;
    zklua_handle_t *handle = luaL_checkudata(L, 1, ZKLUA_METATABLE_NAME);
    if (_zklua_check_handle(L, handle)) {
        recv_timeout = zoo_recv_timeout(handle->zh);
        lua_pushinteger(L, recv_timeout);
        return 1;
    } else {
        return luaL_error(L, "unable to get recv_timeout.");
    }

}

static int zklua_get_context(lua_State *L)
{
    const char *context = NULL;
    zklua_handle_t *handle = luaL_checkudata(L, 1, ZKLUA_METATABLE_NAME);
    if (_zklua_check_handle(L, handle)) {
        context = zoo_get_context(handle->zh);
        lua_pushstring(L, context);
        return 1;
    } else {
        return luaL_error(L, "unable to get zookeeper handle context.");
    }
}

static int zklua_set_context(lua_State *L)
{
    size_t context_len = 0;
    char *context = NULL;
    zklua_handle_t *handle = luaL_checkudata(L, 1, ZKLUA_METATABLE_NAME);
    if (_zklua_check_handle(L, handle)) {
        context = (char *)luaL_checklstring(L, -1, &context_len);
        zoo_set_context(handle->zh, context);
        return 0;
    } else {
        return luaL_error(L, "unable to get zookeeper handle context.");
    }
}

static int zklua_set_watcher(lua_State *L)
{
    zklua_handle_t *handle = luaL_checkudata(L, 1, ZKLUA_METATABLE_NAME);
    if (_zklua_check_handle(L, handle)) {
        lua_pushlightuserdata(L, (void *)&watcher_fn_key);
        lua_gettable(L, LUA_REGISTRYINDEX);
        luaL_checktype(L, 2, LUA_TFUNCTION);
        _zklua_save_watcherfn(L, 2);
        return 1;
    } else {
        return luaL_error(L, "unable to set zookeeper new watcher_fn.");
    }
}

static int zklua_get_connected_host(lua_State *L)
{
    unsigned short port = 0;

    struct sockaddr *paddr = NULL;
    struct sockaddr saddr;
    socklen_t socklen = 0;

    zklua_handle_t *handle = luaL_checkudata(L, 1, ZKLUA_METATABLE_NAME);
    if (_zklua_check_handle(L, handle)) {
        paddr = zookeeper_get_connected_host(handle->zh, &saddr, &socklen);
        if (paddr != NULL) {
            if (socklen == sizeof(struct sockaddr_in)) {
                char addrstr[INET_ADDRSTRLEN] = {0};
                port = ntohs(((struct sockaddr_in *)paddr)->sin_port);
                inet_ntop(AF_INET, &(((struct sockaddr_in *)paddr)->sin_addr), addrstr, INET_ADDRSTRLEN);
                lua_pushstring(L, addrstr);
                lua_pushinteger(L, port);
                return 2;
            } else if (socklen == sizeof(struct sockaddr_in6)) {
                char addr6str[INET6_ADDRSTRLEN] = {0};
                port = ntohs(((struct sockaddr_in6 *)paddr)->sin6_port);
                inet_ntop(AF_INET6, &(((struct sockaddr_in6 *)paddr)->sin6_addr), addr6str, INET6_ADDRSTRLEN);
                lua_pushstring(L, addr6str);
                lua_pushinteger(L, port);
                return 2;
            } else {
                return luaL_error(L, "unsupported network sockaddr type.");
            }
        }
    } else {
        return luaL_error(L, "unable to get connected host.");
    }
    return 0;
}

/**
 * TODO: implement zklua_interest later.
 **/
static int zklua_interest(lua_State *L)
{
    zklua_handle_t *handle = luaL_checkudata(L, 1, ZKLUA_METATABLE_NAME);
    return 0;
}

/**
 * TODO: implement zklua_process later.
 **/
static int zklua_process(lua_State *L)
{
    zklua_handle_t *handle = luaL_checkudata(L, 1, ZKLUA_METATABLE_NAME);
    return 0;
}

static int zklua_state(lua_State *L)
{
    int ret = 0;
    zklua_handle_t *handle = luaL_checkudata(L, 1, ZKLUA_METATABLE_NAME);
    if (_zklua_check_handle(L, handle)) {
        ret = zoo_state(handle->zh);
        lua_pushinteger(L, ret);
        return 1;
    } else {
        return luaL_error(L, "invalid zookeeper handle.");
    }
}

/**
 * create a node asynchronously.
 **/
static int zklua_acreate(lua_State *L)
{
    size_t path_len = 0, value_len=0;
    const char *path = NULL;
    const char *value = NULL;
    const char *data = NULL;
    struct ACL_vector acl;
    zklua_completion_data_t *cdata = NULL;
    int flags = 0;
    int ret = -1;

    zklua_handle_t *handle = luaL_checkudata(L, 1, ZKLUA_METATABLE_NAME);
    if (_zklua_check_handle(L, handle)) {
        path = luaL_checklstring(L, 2, &path_len);
        value = luaL_checklstring(L, 3, &value_len);
        if (!_zklua_parse_acls(L, 4, &acl)) return luaL_error(L,
                "invalid ACL format.");
        flags = luaL_checkint(L, 5);
        luaL_checktype(L, 6, LUA_TFUNCTION);
        data = luaL_checkstring(L, 7);
        cdata = (zklua_completion_data_t *)malloc(sizeof(zklua_completion_data_t));
        cdata->L = lua_newthread(L);
        cdata->data= data;
        lua_pushvalue(L, 6);
        lua_xmove(L, cdata->L, 1);
        ret = zoo_acreate(handle->zh, path, value, value_len,
                (const struct ACL_vector *)&acl, flags,
                string_completion_dispatch, cdata);
        lua_pop(L, 1); // popup the thread.
        lua_pushinteger(L, ret);
        _zklua_free_acls(&acl);
        return 1;
    } else {
        return luaL_error(L, "invalid zookeeper handle.");
    }
}

/**
 * delete a node asynchronously.
 **/
static int zklua_adelete(lua_State *L)
{
    size_t path_len = 0;
    const char *path = NULL;
    int version = -1;
    const char *data = NULL;
    zklua_completion_data_t *cdata = NULL;
    int ret = -1;

    zklua_handle_t *handle = luaL_checkudata(L, 1, ZKLUA_METATABLE_NAME);
    if (_zklua_check_handle(L, handle)) {
        path = luaL_checklstring(L, 2, &path_len);
        version = luaL_checkint(L, 3);
        luaL_checktype(L, 4, LUA_TFUNCTION);
        data = luaL_checkstring(L, 5);
        cdata = (zklua_completion_data_t *)malloc(sizeof(zklua_completion_data_t));
        cdata->L = lua_newthread(L);
        cdata->data= data;
        lua_pushvalue(L, 4);
        lua_xmove(L, cdata->L, 1);
        ret = zoo_adelete(handle->zh, path, version, void_completion_dispatch, cdata);
        lua_pop(L, 1); // popup the thread.
        // printf("zklua_adelete: %s\n", lua_typename(L, lua_type(L, 1)));
        lua_pushnumber(L, ret);
        return 1;
    } else {
        return luaL_error(L, "invalid zookeeper handle.");
    }
}

/**
 * checks the existence of a node in zookeeper.
 **/
static int zklua_aexists(lua_State *L)
{
    size_t path_len = 0;
    const char *path = NULL;
    int watch = 0;
    const char *data = NULL;
    zklua_completion_data_t *cdata = NULL;
    int ret = -1;

    zklua_handle_t *handle = luaL_checkudata(L, 1, ZKLUA_METATABLE_NAME);
    if (_zklua_check_handle(L, handle)) {
        path = luaL_checklstring(L, 2, &path_len);
        watch = luaL_checkint(L, 3);
        luaL_checktype(L, 4, LUA_TFUNCTION);
        data = luaL_checkstring(L, 5);
        cdata = (zklua_completion_data_t *)malloc(sizeof(zklua_completion_data_t));
        cdata->L = lua_newthread(L);
        cdata->data= data;
        lua_pushvalue(L, 4);
        lua_xmove(L, cdata->L, 1);
        ret = zoo_aexists(handle->zh, path, watch,
                stat_completion_dispatch, cdata);
        lua_pop(L, 1); // popup the thread.
        lua_pushinteger(L, ret);
        return 1;
    } else {
        return luaL_error(L, "invalid zookeeper handle.");
    }
}

/**
 * checks the existence of a node in zookeeper.
 **/
static int zklua_awexists(lua_State *L)
{
    size_t path_len = 0;
    const char *real_local_watcherctx = NULL;
    const char *path = NULL;
    int watch = 0;
    const char *data = NULL;
    zklua_local_watcher_context_t *wrapper = NULL;
    zklua_completion_data_t *cdata = NULL;
    int ret = -1;
    int zhref = 0;
    int cbref = 0;

    zklua_handle_t *handle = luaL_checkudata(L, 1, ZKLUA_METATABLE_NAME);
    zhref = _zklua_ref(L, 1);
    if (_zklua_check_handle(L, handle)) {
        path = luaL_checklstring(L, 2, &path_len);
        luaL_checktype(L, 3, LUA_TFUNCTION);
        cbref = _zklua_ref(L, 3);
        real_local_watcherctx = luaL_checkstring(L, 4);
        wrapper = _zklua_local_watcher_context_init(L,
                (void *)real_local_watcherctx, zhref, cbref);
        luaL_checktype(L, 5, LUA_TFUNCTION);
        data = luaL_checkstring(L, 6);
        cdata = (zklua_completion_data_t *)malloc(sizeof(zklua_completion_data_t));
        cdata->L = lua_newthread(L);
        cdata->data= data;
        lua_pushvalue(L, 5);
        lua_xmove(L, cdata->L, 1);
        ret = zoo_awexists(handle->zh, path, local_watcher_dispatch,
                (void *)wrapper, stat_completion_dispatch, cdata);
        lua_pop(L, 1); // popup the thread.
        lua_pushinteger(L, ret);
        return 1;
    } else {
        return luaL_error(L, "invalid zookeeper handle.");
    }
}

/**
 * gets the data associated with a node.
 **/
static int zklua_aget(lua_State *L)
{
    size_t path_len = 0;
    const char *path = NULL;
    int watch = 0;
    const char *data = NULL;
    zklua_completion_data_t *cdata = NULL;
    int ret = -1;

    zklua_handle_t *handle = luaL_checkudata(L, 1, ZKLUA_METATABLE_NAME);
    if (_zklua_check_handle(L, handle)) {
        path = luaL_checklstring(L, 2, &path_len);
        watch = luaL_checkint(L, 3);
        luaL_checktype(L, 4, LUA_TFUNCTION);
        data = luaL_checkstring(L, 5);
        cdata = (zklua_completion_data_t *)malloc(sizeof(zklua_completion_data_t));
        cdata->L = lua_newthread(L);
        cdata->data= data;
        lua_pushvalue(L, 4);
        lua_xmove(L, cdata->L, 1);
        ret = zoo_aget(handle->zh, path, watch,
                data_completion_dispatch, cdata);
        lua_pop(L, 1); // popup the thread.
        lua_pushinteger(L, ret);
        return 1;
    } else {
        return luaL_error(L, "invalid zookeeper handle.");
    }
}

static int zklua_awget(lua_State *L)
{
    size_t path_len = 0;
    const char *real_local_watcherctx = NULL;
    const char *path = NULL;
    int watch = 0;
    const char *data = NULL;
    zklua_local_watcher_context_t *wrapper = NULL;
    zklua_completion_data_t *cdata = NULL;
    int ret = -1;
    int zhref = 0;
    int cbref = 0;

    zklua_handle_t *handle = luaL_checkudata(L, 1, ZKLUA_METATABLE_NAME);
    zhref = _zklua_ref(L, 1);
    if (_zklua_check_handle(L, handle)) {
        path = luaL_checklstring(L, 2, &path_len);
        luaL_checktype(L, 3, LUA_TFUNCTION);
        cbref = _zklua_ref(L, 3);
        real_local_watcherctx = luaL_checkstring(L, 4);
        wrapper = _zklua_local_watcher_context_init(L,
                (void *)real_local_watcherctx, zhref, cbref);
        luaL_checktype(L, 5, LUA_TFUNCTION);
        data = luaL_checkstring(L, 6);
        cdata = (zklua_completion_data_t *)malloc(sizeof(zklua_completion_data_t));
        cdata->L = lua_newthread(L);
        cdata->data= data;
        lua_pushvalue(L, 5);
        lua_xmove(L, cdata->L, 1);
        ret = zoo_awget(handle->zh, path, local_watcher_dispatch,
                (void *)wrapper, data_completion_dispatch, cdata);
        lua_pop(L, 1); // popup the thread.
        lua_pushinteger(L, ret);
        return 1;
    } else {
        return luaL_error(L, "invalid zookeeper handle.");
    }
}

static int zklua_aset(lua_State *L)
{
    size_t path_len = 0;
    size_t buffer_len = 0;
    const char *path = NULL;
    const char *buffer = NULL;
    const char *data = NULL;
    zklua_completion_data_t *cdata = NULL;
    int version = 0;
    int ret = -1;

    zklua_handle_t *handle = luaL_checkudata(L, 1, ZKLUA_METATABLE_NAME);
    if (_zklua_check_handle(L, handle)) {
        path = luaL_checklstring(L, 2, &path_len);
        buffer = luaL_checklstring(L, 3, &buffer_len);
        version = luaL_checkint(L, 4);
        luaL_checktype(L, 5, LUA_TFUNCTION);
        data = luaL_checkstring(L, 6);
        cdata = (zklua_completion_data_t *)malloc(sizeof(zklua_completion_data_t));
        cdata->L = lua_newthread(L);
        cdata->data= data;
        lua_pushvalue(L, 5);
        lua_xmove(L, cdata->L, 1);
        ret = zoo_aset(handle->zh, path, buffer, buffer_len, version,
                stat_completion_dispatch, cdata);
        lua_pop(L, 1); // popup the thread.
        lua_pushinteger(L, ret);
        return 1;
    } else {
        return luaL_error(L, "invalid zookeeper handle.");
    }
}

static int zklua_aget_children(lua_State *L)
{
    size_t path_len = 0;
    const char *path = NULL;
    int watch = 0;
    const char *data = NULL;
    zklua_completion_data_t *cdata = NULL;
    int ret = -1;

    zklua_handle_t *handle = luaL_checkudata(L, 1, ZKLUA_METATABLE_NAME);
    if (_zklua_check_handle(L, handle)) {
        path = luaL_checklstring(L, 2, &path_len);
        watch = luaL_checkint(L, 3);
        luaL_checktype(L, 4, LUA_TFUNCTION);
        data = luaL_checkstring(L, 5);
        cdata = (zklua_completion_data_t *)malloc(sizeof(zklua_completion_data_t));
        cdata->L = lua_newthread(L);
        cdata->data= data;
        lua_pushvalue(L, 4);
        lua_xmove(L, cdata->L, 1);
        ret = zoo_aget_children(handle->zh, path, watch,
                strings_completion_dispatch, cdata);
        lua_pop(L, 1); // popup the thread.
        lua_pushinteger(L, ret);
        return 1;
    } else {
        return luaL_error(L, "invalid zookeeper handle.");
    }
}

static int zklua_aget_children2(lua_State *L)
{
    size_t path_len = 0;
    const char *path = NULL;
    int watch = 0;
    const char *data = NULL;
    zklua_completion_data_t *cdata = NULL;
    int ret = -1;

    zklua_handle_t *handle = luaL_checkudata(L, 1, ZKLUA_METATABLE_NAME);
    if (_zklua_check_handle(L, handle)) {
        path = luaL_checklstring(L, 2, &path_len);
        watch = luaL_checkint(L, 3);
        luaL_checktype(L, 4, LUA_TFUNCTION);
        data = luaL_checkstring(L, 5);
        cdata = (zklua_completion_data_t *)malloc(sizeof(zklua_completion_data_t));
        cdata->L = lua_newthread(L);
        cdata->data= data;
        lua_pushvalue(L, 4);
        lua_xmove(L, cdata->L, 1);
        ret = zoo_aget_children2(handle->zh, path, watch,
                strings_stat_completion_dispatch, cdata);
        lua_pop(L, 1); // popup the thread.
        lua_pushinteger(L, ret);
        return 1;
    } else {
        return luaL_error(L, "invalid zookeeper handle.");
    }
}

static int zklua_awget_children(lua_State *L)
{
    size_t path_len = 0;
    const char *real_local_watcherctx = NULL;
    const char *path = NULL;
    int watch = 0;
    const char *data = NULL;
    zklua_local_watcher_context_t *wrapper = NULL;
    zklua_completion_data_t *cdata = NULL;
    int ret = -1;
    int zhref = 0;
    int cbref = 0;

    zklua_handle_t *handle = luaL_checkudata(L, 1, ZKLUA_METATABLE_NAME);
    zhref = _zklua_ref(L, 1);
    if (_zklua_check_handle(L, handle)) {
        path = luaL_checklstring(L, 2, &path_len);
        luaL_checktype(L, 3, LUA_TFUNCTION);
        cbref = _zklua_ref(L, 3);
        real_local_watcherctx = luaL_checkstring(L, 4);
        wrapper = _zklua_local_watcher_context_init(L,
                (void *)real_local_watcherctx, zhref, cbref);
        luaL_checktype(L, 5, LUA_TFUNCTION);
        data = luaL_checkstring(L, 6);
        cdata = (zklua_completion_data_t *)malloc(sizeof(zklua_completion_data_t));
        cdata->L = lua_newthread(L);
        cdata->data= data;
        lua_pushvalue(L, 5);
        lua_xmove(L, cdata->L, 1);
        ret = zoo_awget_children(handle->zh, path, local_watcher_dispatch,
                (void *)wrapper, strings_completion_dispatch, cdata);
        lua_pop(L, 1); // popup the thread.
        lua_pushinteger(L, ret);
        return 1;
    } else {
        return luaL_error(L, "invalid zookeeper handle.");
    }
}

static int zklua_awget_children2(lua_State *L)
{
    size_t path_len = 0;
    const char *real_local_watcherctx = NULL;
    const char *path = NULL;
    int watch = 0;
    const char *data = NULL;
    zklua_local_watcher_context_t *wrapper = NULL;
    zklua_completion_data_t *cdata = NULL;
    int ret = -1;
    int zhref = 0;
    int cbref = 0;

    zklua_handle_t *handle = luaL_checkudata(L, 1, ZKLUA_METATABLE_NAME);
    zhref = _zklua_ref(L, 1);
    if (_zklua_check_handle(L, handle)) {
        path = luaL_checklstring(L, 2, &path_len);
        luaL_checktype(L, 3, LUA_TFUNCTION);
        cbref = _zklua_ref(L, 3);
        real_local_watcherctx = luaL_checkstring(L, 4);
        wrapper = _zklua_local_watcher_context_init(L,
                (void *)real_local_watcherctx, zhref, cbref);
        luaL_checktype(L, 5, LUA_TFUNCTION);
        data = luaL_checkstring(L, 6);
        cdata = (zklua_completion_data_t *)malloc(sizeof(zklua_completion_data_t));
        cdata->L = lua_newthread(L);
        cdata->data= data;
        lua_pushvalue(L, 5);
        lua_xmove(L, cdata->L, 1);
        ret = zoo_awget_children2(handle->zh, path, local_watcher_dispatch,
                (void *)wrapper, strings_stat_completion_dispatch, cdata);
        lua_pop(L, 1); // popup the thread.
        lua_pushinteger(L, ret);
        return 1;
    } else {
        return luaL_error(L, "invalid zookeeper handle.");
    }
}

static int zklua_async(lua_State *L)
{
    size_t path_len = 0;
    const char *path = NULL;
    int watch = 0;
    const char *data = NULL;
    zklua_completion_data_t *cdata = NULL;
    int ret = -1;

    zklua_handle_t *handle = luaL_checkudata(L, 1, ZKLUA_METATABLE_NAME);
    if (_zklua_check_handle(L, handle)) {
        path = luaL_checklstring(L, 2, &path_len);
        luaL_checktype(L, 3, LUA_TFUNCTION);
        data = luaL_checkstring(L, 4);
        cdata = (zklua_completion_data_t *)malloc(sizeof(zklua_completion_data_t));
        cdata->L = lua_newthread(L);
        cdata->data= data;
        lua_pushvalue(L, 3);
        lua_xmove(L, cdata->L, 1);
        ret = zoo_async(handle->zh, path,
                string_completion_dispatch, cdata);
        lua_pop(L, 1); // popup the thread.
        lua_pushinteger(L, ret);
        return 1;
    } else {
        return luaL_error(L, "invalid zookeeper handle.");
    }
}

static int zklua_aget_acl(lua_State *L)
{
    size_t path_len = 0;
    const char *path = NULL;
    const char *data = NULL;
    zklua_completion_data_t *cdata = NULL;
    int ret = -1;

    zklua_handle_t *handle = luaL_checkudata(L, 1, ZKLUA_METATABLE_NAME);
    if (_zklua_check_handle(L, handle)) {
        path = luaL_checklstring(L, 2, &path_len);
        luaL_checktype(L, 3, LUA_TFUNCTION);
        data = luaL_checkstring(L, 4);
        cdata = (zklua_completion_data_t *)malloc(sizeof(zklua_completion_data_t));
        cdata->L = lua_newthread(L);
        cdata->data= data;
        lua_pushvalue(L, 3);
        lua_xmove(L, cdata->L, 1);
        ret = zoo_aget_acl(handle->zh, path,
                acl_completion_dispatch, cdata);
        lua_pop(L, 1); // popup the thread.
        lua_pushinteger(L, ret);
        return 1;
    } else {
        return luaL_error(L, "invalid zookeeper handle.");
    }
}

static int zklua_aset_acl(lua_State *L)
{
    size_t path_len = 0;
    const char *path = NULL;
    const char *data = NULL;
    struct ACL_vector acl;
    zklua_completion_data_t *cdata = NULL;
    int version = 0;
    int ret = -1;

    zklua_handle_t *handle = luaL_checkudata(L, 1, ZKLUA_METATABLE_NAME);
    if (_zklua_check_handle(L, handle)) {
        path = luaL_checklstring(L, 2, &path_len);
        version = luaL_checkint(L, 3);
        if (!_zklua_parse_acls(L, 4, &acl)) return luaL_error(L,
                "invalid ACL format.");
        luaL_checktype(L, 5, LUA_TFUNCTION);
        data = luaL_checkstring(L, 6);
        cdata = (zklua_completion_data_t *)malloc(sizeof(zklua_completion_data_t));
        cdata->L = lua_newthread(L);
        cdata->data= data;
        lua_pushvalue(L, 5);
        lua_xmove(L, cdata->L, 1);
        ret = zoo_aset_acl(handle->zh, path, version, &acl,
                void_completion_dispatch, cdata);
        lua_pop(L, 1); // popup the thread.
        lua_pushinteger(L, ret);
        _zklua_free_acls(&acl);
        return 1;
    } else {
        return luaL_error(L, "invalid zookeeper handle.");
    }
}

static int zklua_error(lua_State *L)
{
    int code = luaL_checkint(L, -1);
    const char *errstr = zerror(code);
    lua_pushstring(L, errstr);
    return 1;
}

static int zklua_add_auth(lua_State *L)
{
    size_t cert_len = 0;
    const char *scheme = NULL;
    const char *cert = NULL;
    const char *data = NULL;
    zklua_completion_data_t *cdata = NULL;
    int ret = -1;

    zklua_handle_t *handle = luaL_checkudata(L, 1, ZKLUA_METATABLE_NAME);
    if (_zklua_check_handle(L, handle)) {
        scheme = luaL_checkstring(L, 2);
        cert = luaL_checklstring(L, 3, &cert_len);
        luaL_checktype(L, 4, LUA_TFUNCTION);
        data = luaL_checkstring(L, 5);
        cdata = (zklua_completion_data_t *)malloc(sizeof(zklua_completion_data_t));
        cdata->L = lua_newthread(L);
        cdata->data= data;
        lua_pushvalue(L, 4);
        lua_xmove(L, cdata->L, 1);
        ret = zoo_add_auth(handle->zh, scheme, cert, cert_len,
                void_completion_dispatch, cdata);
        lua_pop(L, 1); // popup the thread.
        lua_pushinteger(L, ret);
        return 1;
    } else {
        return luaL_error(L, "invalid zookeeper handle.");
    }
}

static int zklua_is_unrecoverable(lua_State *L)
{
    int ret = 0;
    zklua_handle_t *handle = luaL_checkudata(L, 1, ZKLUA_METATABLE_NAME);
    if (handle->zh != NULL) {
        ret = is_unrecoverable(handle->zh);
        lua_pushinteger(L, ret);
        return 1;
    } else {
        return luaL_error(L, "invalid zookeeper handle.");
    }
}

static int zklua_set_debug_level(lua_State *L)
{
    int level = luaL_checkint(L, -1);
    switch(level) {
        case ZOO_LOG_LEVEL_ERROR:
            zoo_set_debug_level(ZOO_LOG_LEVEL_ERROR);
            break;
        case ZOO_LOG_LEVEL_WARN:
            zoo_set_debug_level(ZOO_LOG_LEVEL_WARN);
            break;
        case ZOO_LOG_LEVEL_INFO:
            zoo_set_debug_level(ZOO_LOG_LEVEL_INFO);
            break;
        case ZOO_LOG_LEVEL_DEBUG:
            zoo_set_debug_level(ZOO_LOG_LEVEL_DEBUG);
            break;
        default:
            return luaL_error(L, "invalid arguments: "
                    "unsupported log level specified.");
    }
    return 0;
}

static int zklua_set_log_stream(lua_State *L)
{
    size_t stream_len = 0;
    const char *stream = luaL_checklstring(L, -1, &stream_len);
    zklua_log_stream = fopen(stream, "w+");
    if (zklua_log_stream == NULL) return luaL_error(L,
            "unable to open the specified file %s.", stream);
    zoo_set_log_stream(zklua_log_stream);
    return 0;
}

static int zklua_deterministic_conn_order(lua_State *L)
{
    size_t yesorno_len = 0;
    const char *yesorno = luaL_checklstring(L, 1, &yesorno_len);
    if (strcasecmp(yesorno, "yes") || strcasecmp(yesorno, "true")) {
        zoo_deterministic_conn_order(1);
    } else if (strcasecmp(yesorno, "no") || strcasecmp(yesorno, "false")) {
        zoo_deterministic_conn_order(0);
    } else {
        return luaL_error(L, "invalid argument: please choose "
                "(yes, no) or (true, false).");
    }
    return 0;
}

static int zklua_create(lua_State *L)
{
    size_t path_len = 0, value_len=0;
    const char *path = NULL;
    const char *value = NULL;
    const char *data = NULL;
    char path_buffer[ZKLUA_MAX_PATH_BUFFER_SIZE] = {0};
    struct ACL_vector acl;
    int flags = 0;
    int ret = -1;

    zklua_handle_t *handle = luaL_checkudata(L, 1, ZKLUA_METATABLE_NAME);
    if (_zklua_check_handle(L, handle)) {
        path = luaL_checklstring(L, 2, &path_len);
        value = luaL_checklstring(L, 3, &value_len);
        if (!_zklua_parse_acls(L, 4, &acl)) return luaL_error(L,
                "invalid ACL format.");
        flags = luaL_checkint(L, 5);
        ret = zoo_create(handle->zh, path, value, value_len,
                (const struct ACL_vector *)&acl, flags,
                path_buffer, ZKLUA_MAX_PATH_BUFFER_SIZE);
        lua_pushinteger(L, ret);
        lua_pushstring(L, path_buffer);
        _zklua_free_acls(&acl);
        return 2;
    } else {
        return luaL_error(L, "invalid zookeeper handle.");
    }
}

static int zklua_delete(lua_State *L)
{
    size_t path_len = 0;
    const char *path = NULL;
    int version = 0;
    int ret = -1;

    zklua_handle_t *handle = luaL_checkudata(L, 1, ZKLUA_METATABLE_NAME);
    if (_zklua_check_handle(L, handle)) {
        path = luaL_checklstring(L, 2, &path_len);
        version = luaL_checkint(L, 3);
        ret = zoo_delete(handle->zh, path, version);
        lua_pushinteger(L, ret);
        return 1;
    } else {
        return luaL_error(L, "invalid zookeeper handle.");
    }
}

static int zklua_exists(lua_State *L)
{
    size_t path_len = 0;
    const char *path = NULL;
    int watch = 0;
    struct Stat stat;
    int ret = -1;

    zklua_handle_t *handle = luaL_checkudata(L, 1, ZKLUA_METATABLE_NAME);
    if (_zklua_check_handle(L, handle)) {
        path = luaL_checklstring(L, 2, &path_len);
        watch = luaL_checkint(L, 3);
        ret = zoo_exists(handle->zh, path, watch, &stat);
        lua_pushinteger(L, ret);
        _zklua_build_stat(L, &stat);
        return 2;
    } else {
        return luaL_error(L, "invalid zookeeper handle.");
    }
}

static int zklua_wexists(lua_State *L)
{
    size_t path_len = 0;
    const char *real_local_watcherctx = NULL;
    const char *path = NULL;
    int watch = 0;
    zklua_local_watcher_context_t *wrapper = NULL;
    struct Stat stat;
    int ret = -1;
    int zhref = 0;
    int cbref = 0;

    zklua_handle_t *handle = luaL_checkudata(L, 1, ZKLUA_METATABLE_NAME);
    zhref = _zklua_ref(L, 1);
    if (_zklua_check_handle(L, handle)) {
        path = luaL_checklstring(L, 2, &path_len);
        luaL_checktype(L, 3, LUA_TFUNCTION);
        cbref = _zklua_ref(L, 3);
        real_local_watcherctx = luaL_checkstring(L, 4);
        wrapper = _zklua_local_watcher_context_init(L,
                (void *)real_local_watcherctx, zhref, cbref);
        ret = zoo_wexists(handle->zh, path, local_watcher_dispatch,
                (void *)wrapper, &stat);
        lua_pushinteger(L, ret);
        _zklua_build_stat(L, &stat);
        return 2;
    } else {
        return luaL_error(L, "invalid zookeeper handle.");
    }
}

static int zklua_get(lua_State *L)
{
    size_t path_len = 0;
    const char *path = NULL;
    char *buffer = malloc(2048);
    int buffer_len = 2048;
    int watch = 0;
    struct Stat stat;
    int ret = -1;

    zklua_handle_t *handle = luaL_checkudata(L, 1, ZKLUA_METATABLE_NAME);
    if (_zklua_check_handle(L, handle)) {
        path = luaL_checklstring(L, 2, &path_len);
        watch = luaL_checkint(L, 3);
        ret = zoo_get(handle->zh, path, watch, buffer, &buffer_len, &stat);
        lua_pushinteger(L, ret);
        lua_pushlstring(L, buffer, buffer_len);
        free(buffer);
        _zklua_build_stat(L, &stat);
        return 3;
    } else {
        return luaL_error(L, "invalid zookeeper handle.");
    }
}

static int zklua_wget(lua_State *L)
{
    size_t path_len = 0;
    const char *real_local_watcherctx = NULL;
    const char *path = NULL;
    char *buffer = NULL;
    int buffer_len = 0;
    zklua_local_watcher_context_t *wrapper = NULL;
    struct Stat stat;
    int ret = -1;
    int zhref = 0;
    int cbref = 0;

    zklua_handle_t *handle = luaL_checkudata(L, 1, ZKLUA_METATABLE_NAME);
    zhref = _zklua_ref(L, 1);
    if (_zklua_check_handle(L, handle)) {
        path = luaL_checklstring(L, 2, &path_len);
        luaL_checktype(L, 3, LUA_TFUNCTION);
        cbref = _zklua_ref(L, 3);
        real_local_watcherctx = luaL_checkstring(L, 4);
        wrapper = _zklua_local_watcher_context_init(L,
                (void *)real_local_watcherctx, zhref, cbref);
        ret = zoo_wget(handle->zh, path, local_watcher_dispatch,
                (void *)wrapper, buffer, &buffer_len, &stat);
        lua_pushinteger(L, ret);
        lua_pushlstring(L, buffer, buffer_len);
        _zklua_build_stat(L, &stat);
        return 3;
    } else {
        return luaL_error(L, "invalid zookeeper handle.");
    }
}

static int zklua_set(lua_State *L)
{
    size_t path_len = 0, buffer_len = 0;
    const char *path = NULL;
    const char *buffer = NULL;
    int version = 0;
    int ret = -1;

    zklua_handle_t *handle = luaL_checkudata(L, 1, ZKLUA_METATABLE_NAME);
    if (_zklua_check_handle(L, handle)) {
        path = luaL_checklstring(L, 2, &path_len);
        buffer = luaL_checklstring(L, 3, &buffer_len);
        version = luaL_checkint(L, 4);
        ret = zoo_set(handle->zh, path, buffer, buffer_len, version);
        lua_pushinteger(L, ret);
        return 1;
    } else {
        return luaL_error(L, "invalid zookeeper handle.");
    }
}

static int zklua_set2(lua_State *L)
{
    size_t path_len = 0, buffer_len = 0;
    const char *path = NULL;
    const char *buffer = NULL;
    int version = 0;
    struct Stat stat;
    int ret = -1;

    zklua_handle_t *handle = luaL_checkudata(L, 1, ZKLUA_METATABLE_NAME);
    if (_zklua_check_handle(L, handle)) {
        path = luaL_checklstring(L, 2, &path_len);
        buffer = luaL_checklstring(L, 3, &buffer_len);
        version = luaL_checkint(L, 4);
        ret = zoo_set2(handle->zh, path, buffer, buffer_len, version, &stat);
        lua_pushinteger(L, ret);
        _zklua_build_stat(L, &stat);
        return 2;
    } else {
        return luaL_error(L, "invalid zookeeper handle.");
    }
}

static int zklua_get_children(lua_State *L)
{
    size_t path_len = 0;
    const char *path = NULL;
    char *buffer = NULL;
    int buffer_len = 0;
    int watch = 0;
    struct String_vector strings;
    int ret = -1;

    zklua_handle_t *handle = luaL_checkudata(L, 1, ZKLUA_METATABLE_NAME);
    if (_zklua_check_handle(L, handle)) {
        path = luaL_checklstring(L, 2, &path_len);
        watch = luaL_checkint(L, 3);
        ret = zoo_get_children(handle->zh, path, watch,  &strings);
        lua_pushinteger(L, ret);
        _zklua_build_string_vector(L, &strings);
        return 2;
    } else {
        return luaL_error(L, "invalid zookeeper handle.");
    }
}

static int zklua_wget_children(lua_State *L)
{
    size_t path_len = 0;
    const char *real_local_watcherctx = NULL;
    const char *path = NULL;
    zklua_local_watcher_context_t *wrapper = NULL;
    struct String_vector strings;
    int ret = -1;
    int zhref = 0;
    int cbref = 0;

    zklua_handle_t *handle = luaL_checkudata(L, 1, ZKLUA_METATABLE_NAME);
    zhref = _zklua_ref(L, 1);
    if (_zklua_check_handle(L, handle)) {
        path = luaL_checklstring(L, 2, &path_len);
        luaL_checktype(L, 3, LUA_TFUNCTION);
        cbref = _zklua_ref(L, 3);
        real_local_watcherctx = luaL_checkstring(L, 4);
        wrapper = _zklua_local_watcher_context_init(L,
                (void *)real_local_watcherctx, zhref, cbref);
        ret = zoo_wget_children(handle->zh, path, local_watcher_dispatch,
                (void *)wrapper, &strings);
        lua_pushinteger(L, ret);
        _zklua_build_string_vector(L, &strings);
        return 2;
    } else {
        return luaL_error(L, "invalid zookeeper handle.");
    }
}

static int zklua_get_children2(lua_State *L)
{
    size_t path_len = 0;
    const char *path = NULL;
    char *buffer = NULL;
    int buffer_len = 0;
    int watch = 0;
    struct String_vector strings;
    struct Stat stat;
    int ret = -1;

    zklua_handle_t *handle = luaL_checkudata(L, 1, ZKLUA_METATABLE_NAME);
    if (_zklua_check_handle(L, handle)) {
        path = luaL_checklstring(L, 2, &path_len);
        watch = luaL_checkint(L, 3);
        ret = zoo_get_children2(handle->zh, path, watch, &strings, &stat);
        lua_pushinteger(L, ret);
        _zklua_build_string_vector(L, &strings);
        _zklua_build_stat(L, &stat);
        return 3;
    } else {
        return luaL_error(L, "invalid zookeeper handle.");
    }
}

static int zklua_wget_children2(lua_State *L)
{
    size_t path_len = 0;
    const char *real_local_watcherctx = NULL;
    const char *path = NULL;
    zklua_local_watcher_context_t *wrapper = NULL;
    struct String_vector strings;
    struct Stat stat;
    int ret = -1;
    int zhref = 0;
    int cbref = 0;

    zklua_handle_t *handle = luaL_checkudata(L, 1, ZKLUA_METATABLE_NAME);
    zhref = _zklua_ref(L, 1);
    if (_zklua_check_handle(L, handle)) {
        path = luaL_checklstring(L, 2, &path_len);
        luaL_checktype(L, 3, LUA_TFUNCTION);
        cbref = _zklua_ref(L, 3);
        real_local_watcherctx = luaL_checkstring(L, 4);
        wrapper = _zklua_local_watcher_context_init(L,
                (void *)real_local_watcherctx, zhref, cbref);
        ret = zoo_wget_children2(handle->zh, path, local_watcher_dispatch,
                (void *)wrapper, &strings, &stat);
        lua_pushinteger(L, ret);
        _zklua_build_string_vector(L, &strings);
        _zklua_build_stat(L, &stat);
        return 3;
    } else {
        return luaL_error(L, "invalid zookeeper handle.");
    }
}

static int zklua_get_acl(lua_State *L)
{
    size_t path_len = 0;
    const char *path = NULL;
    struct ACL_vector acl;
    struct Stat stat;
    int ret = -1;

    zklua_handle_t *handle = luaL_checkudata(L, 1, ZKLUA_METATABLE_NAME);
    if (_zklua_check_handle(L, handle)) {
        path = luaL_checklstring(L, 2, &path_len);
        ret = zoo_get_acl(handle->zh, path, &acl, &stat);
        lua_pushinteger(L, ret);
        _zklua_build_acls(L, &acl);
        _zklua_build_stat(L, &stat);
        return 3;
    } else {
        return luaL_error(L, "invalid zookeeper handle.");
    }
}

static int zklua_set_acl(lua_State *L)
{
    size_t path_len = 0;
    const char *path = NULL;
    int version = 0;
    struct ACL_vector acl;
    struct Stat stat;
    int ret = -1;

    zklua_handle_t *handle = luaL_checkudata(L, 1, ZKLUA_METATABLE_NAME);
    if (_zklua_check_handle(L, handle)) {
        path = luaL_checklstring(L, 2, &path_len);
        version = luaL_checkint(L, 3);
        _zklua_parse_acls(L, 4, &acl);
        ret = zoo_set_acl(handle->zh, path, version, &acl);
        lua_pushinteger(L, ret);
        _zklua_free_acls(&acl);
        return 1;
    } else {
        return luaL_error(L, "invalid zookeeper handle.");
    }
}

static const luaL_Reg zklua[] =
{
    {"init", zklua_init},
    {"close", zklua_close},
    {"client_id", zklua_client_id},
    {"recv_timeout", zklua_recv_timeout},
    {"get_context", zklua_get_context},
    {"set_watcher", zklua_set_watcher},
//    {"get_connected_host", zklua_get_connected_host},
    {"interest", zklua_interest},
    {"process", zklua_process},
    {"state", zklua_state},
    {"acreate", zklua_acreate},
    {"adelete", zklua_adelete},
    {"aexists", zklua_aexists},
    {"awexists", zklua_awexists},
    {"aget", zklua_aget},
    {"awget", zklua_awget},
    {"aset", zklua_aset},
    {"aget_children", zklua_aget_children},
    {"aget_children2", zklua_aget_children2},
    {"awget_children", zklua_awget_children},
    {"awget_children2", zklua_awget_children2},
    {"async", zklua_async},
    {"aget_acl", zklua_aget_acl},
    {"aset_acl", zklua_aset_acl},
    {"error", zklua_error},
    {"add_auth", zklua_add_auth},
    {"is_unrecoverable", zklua_is_unrecoverable},
    {"set_debug_level", zklua_set_debug_level},
    {"set_log_stream", zklua_set_log_stream},
    {"deterministic_conn_order", zklua_deterministic_conn_order},
    {"create", zklua_create},
    {"delete", zklua_delete},
    {"exists", zklua_exists},
    {"wexists", zklua_wexists},
    {"get", zklua_get},
    {"wget", zklua_wget},
    {"set", zklua_set},
    {"set2", zklua_set2},
    {"get_children", zklua_get_children},
    {"get_children2", zklua_get_children2},
    {"get_acl", zklua_get_acl},
    {"set_acl", zklua_set_acl},
    {NULL, NULL}
};

#define zklua_register_constant(s)\
    lua_pushinteger(L, s);\
    lua_setfield(L, -2, #s);

int luaopen_zklua(lua_State *L)
{
    luaL_newmetatable(L, ZKLUA_METATABLE_NAME);
#if LUA_VERSION_NUM == 502
    luaL_newlib(L, zklua);
#else
    luaL_register(L, "zklua", zklua);
#endif
    lua_pushliteral (L, "Copyright (C) 2013 Fu Haiping(forhappy)");
    lua_setfield(L, -2, "_COPYRIGHT");
    lua_pushliteral (L, "zklua: lua binding of Apache ZooKeeper");
    lua_setfield(L, -2, "_DESCRIPTION");
    lua_pushliteral (L, "0.1.2");
    lua_setfield(L, -2, "_VERSION");

    /**
     * register zookeeper constants in lua.
     **/
    zklua_register_constant(ZOK);

    /**
     * System and server-side errors.
     * This is never thrown by the server, it shouldn't be used other than
     * to indicate a range. Specifically error codes greater than this
     * value, but lesser than ZAPIERROR, are system errors.
     **/
    zklua_register_constant(ZSYSTEMERROR);
    zklua_register_constant(ZRUNTIMEINCONSISTENCY);
    zklua_register_constant(ZCONNECTIONLOSS);
    zklua_register_constant(ZMARSHALLINGERROR);
    zklua_register_constant(ZUNIMPLEMENTED);
    zklua_register_constant(ZOPERATIONTIMEOUT);
    zklua_register_constant(ZBADARGUMENTS);
    zklua_register_constant(ZINVALIDSTATE);

    /**
     * API errors.
     * This is never thrown by the server, it shouldn't be used other than
     * to indicate a range. Specifically error codes greater than this
     * value are API errors (while values less than this indicate a
     * ZSYSTEMERROR).
     **/
    zklua_register_constant(ZAPIERROR);
    zklua_register_constant(ZNONODE);
    zklua_register_constant(ZNOAUTH);
    zklua_register_constant(ZBADVERSION);
    zklua_register_constant(ZNOCHILDRENFOREPHEMERALS);
    zklua_register_constant(ZNODEEXISTS);
    zklua_register_constant(ZNOTEMPTY);
    zklua_register_constant(ZSESSIONEXPIRED);
    zklua_register_constant(ZINVALIDCALLBACK);
    zklua_register_constant(ZINVALIDACL);
    zklua_register_constant(ZAUTHFAILED);
    zklua_register_constant(ZCLOSING);
    zklua_register_constant(ZNOTHING);
    zklua_register_constant(ZSESSIONMOVED);

    /**
     * ACL Constants.
     **/
    zklua_register_constant(ZOO_PERM_READ);
    zklua_register_constant(ZOO_PERM_WRITE);
    zklua_register_constant(ZOO_PERM_DELETE);
    zklua_register_constant(ZOO_PERM_ADMIN);
    zklua_register_constant(ZOO_PERM_ALL);

    /**
     * Debug Levels.
     **/
    zklua_register_constant(ZOO_LOG_LEVEL_ERROR);
    zklua_register_constant(ZOO_LOG_LEVEL_WARN);
    zklua_register_constant(ZOO_LOG_LEVEL_INFO);
    zklua_register_constant(ZOO_LOG_LEVEL_DEBUG);

    /**
     * Interest Constants.
     **/
    zklua_register_constant(ZOOKEEPER_WRITE);
    zklua_register_constant(ZOOKEEPER_READ);

    /**
     * Create Flags.
     **/
    zklua_register_constant(ZOO_EPHEMERAL);
    zklua_register_constant(ZOO_SEQUENCE);

    /**
     * State Constants.
     **/
    zklua_register_constant(ZOO_EXPIRED_SESSION_STATE);
    zklua_register_constant(ZOO_AUTH_FAILED_STATE);
    zklua_register_constant(ZOO_CONNECTING_STATE);
    zklua_register_constant(ZOO_ASSOCIATING_STATE);
    zklua_register_constant(ZOO_CONNECTED_STATE);

    /**
     * Watch Types.
     **/
    zklua_register_constant(ZOO_CREATED_EVENT);
    zklua_register_constant(ZOO_DELETED_EVENT);
    zklua_register_constant(ZOO_CHANGED_EVENT);
    zklua_register_constant(ZOO_CHILD_EVENT);
    zklua_register_constant(ZOO_SESSION_EVENT);
    zklua_register_constant(ZOO_NOTWATCHING_EVENT);

    return 1;
}
