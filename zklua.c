/*
 * ========================================================================
 *
 *       Filename:  zklua.c
 *
 *    Description:  lua binding of apache zookeeper.
 *
 *        Created:  04/02/2013 05:17:01 PM
 *
 *         Author:  Fu Haiping (forhappy), haipingf@gmail.com
 *        Company:  ICT ( Institute Of Computing Technology, CAS )
 *
 * ========================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "zklua.h"

void watcher_dispatch(zhandle_t *zh, int type, int state,
        const char *path, void *watcherCtx)
{
    zklua_watcher_context_t *wrapper = (zklua_watcher_context_t *)watcherCtx;
    lua_State *L = wrapper->L;
    void *context = wrapper->context;
    lua_pushvalue(L, 2);
    lua_pushinteger(L, type);
    lua_pushinteger(L, state);
    lua_pushstring(L, path);
    if (context != NULL) {
        lua_pushstring(L, context);
        lua_call(L, 4, 0);
    } else {
        lua_call(L, 3, 0);
    }
}

void void_completion_dispatch(int rc, const void *data)
{}

void stat_completion_dispatch(int rc, const struct Stat *stat,
        const void *data)
{}

void data_completion_dispatch(int rc, const char *value, int value_len,
        const struct Stat *stat, const void *data)
{}

void strings_completion_dispatch(int rc, const struct String_vector *strings,
        const void *data)
{}

void strings_stat_completion_t(int rc, const struct String_vector *strings,
        const struct Stat *stat, const void *data)
{}

void string_completion_t(int rc, const char *value, const void *data)
{}

void acl_completion_t(int rc, struct ACL_vector *acl,
        struct Stat *stat, const void *data)
{}

/**
 * return 1 if @host@ has the following format:
 * "127.0.0.1:2081,127.0.0.1:2082,127.0.0.1:2083".
 **/
static int _zklua_check_host(const char *host)
{
    return 1;
}

static zklua_watcher_context_t *_zklua_watcher_context_init(
        lua_State *L, void *data)
{
    zklua_watcher_context_t *wrapper = (zklua_watcher_context_t *)malloc(
        sizeof(zklua_watcher_context_t));
    if (wrapper == NULL) {
        luaL_error(L, "out of memory when zklua trys to "
                "alloc an internal object");
    }
    wrapper->L = L;
    wrapper->context = data;
    return wrapper;
}

static clientid_t *_zklua_clientid_init(
        lua_State *L, int index)
{
    size_t passwd_len = 0;
    const char *clientid_passwd = NULL;
    clientid_t *clientid = NULL;
    clientid = (clientid_t *)malloc(sizeof(clientid_t));
    if (clientid == NULL) {
        luaL_error(L, "out of memory when zklua trys to "
                "alloc an internal object");
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
    zklua_watcher_context_t *wrapper = NULL;

    zklua_handle_t *handle = (zklua_handle_t *)lua_newuserdata(L,
            sizeof(zklua_handle_t));
    luaL_getmetatable(L, ZKLUA_METATABLE_NAME);
    lua_setmetatable(L, -2);

    host = luaL_checklstring(L, 1, &host_len);
    if (_zklua_check_host(host)) {
        return luaL_error(L, "invalid arguments:"
                "host must be a string with format:\n"
                "127.0.0.1:2081,127.0.0.1:2082");
    }
    luaL_checktype(L, 2, LUA_TFUNCTION);
    recv_timeout = luaL_checkint(L, 3);
    switch(top) {
        case 3:
            wrapper = _zklua_watcher_context_init(L, NULL);
            handle->zh = zookeeper_init(host, watcher_dispatch,
                    recv_timeout, 0, wrapper, 0);
            break;
        case 4:
            luaL_checktype(L, 4, LUA_TTABLE);
            clientid = _zklua_clientid_init(L, 4);
            wrapper = _zklua_watcher_context_init(L, NULL);
            handle->zh = zookeeper_init(host, watcher_dispatch,
                    recv_timeout, clientid, wrapper, 0);
            _zklua_clientid_fini(&clientid);
            break;
        case 5:
            luaL_checktype(L, 4, LUA_TTABLE);
            clientid = _zklua_clientid_init(L, 4);
            real_watcher_context = (char *)luaL_checklstring(L, 5, &real_context_len);
            wrapper = _zklua_watcher_context_init(L, real_watcher_context);
            handle->zh = zookeeper_init(host, watcher_dispatch,
                    recv_timeout, clientid, wrapper, 0);
            _zklua_clientid_fini(&clientid);
            break;
        case 6:
            luaL_checktype(L, 4, LUA_TTABLE);
            clientid = _zklua_clientid_init(L, 4);
            real_watcher_context = (char *)luaL_checklstring(L, 5, &real_context_len);
            wrapper = _zklua_watcher_context_init(L, real_watcher_context);
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
        ret = zookeeper_close(handle->zh);
        handle->zh = NULL;
    } else {
        return luaL_error(L, "unable to close the zookeeper handle.");
    }
    lua_pushinteger(L, ret);
    return 1;
}

static int zklua_client_id(lua_State *L)
{
    zklua_handle_t *handle = luaL_checkudata(L, 1, ZKLUA_METATABLE_NAME);
}

static int zklua_recv_timeout(lua_State *L)
{
    zklua_handle_t *handle = luaL_checkudata(L, 1, ZKLUA_METATABLE_NAME);
}

static int zklua_get_context(lua_State *L)
{
    zklua_handle_t *handle = luaL_checkudata(L, 1, ZKLUA_METATABLE_NAME);
}

static int zklua_set_context(lua_State *L)
{
    zklua_handle_t *handle = luaL_checkudata(L, 1, ZKLUA_METATABLE_NAME);
}

static int zklua_set_watcher(lua_State *L)
{
    zklua_handle_t *handle = luaL_checkudata(L, 1, ZKLUA_METATABLE_NAME);
}

static int zklua_get_connected_host(lua_State *L)
{
    zklua_handle_t *handle = luaL_checkudata(L, 1, ZKLUA_METATABLE_NAME);
}

static int zklua_interest(lua_State *L)
{
    zklua_handle_t *handle = luaL_checkudata(L, 1, ZKLUA_METATABLE_NAME);
}

static int zklua_process(lua_State *L)
{
    zklua_handle_t *handle = luaL_checkudata(L, 1, ZKLUA_METATABLE_NAME);
}

static int zklua_state(lua_State *L)
{
    zklua_handle_t *handle = luaL_checkudata(L, 1, ZKLUA_METATABLE_NAME);
}

static int zklua_acreate(lua_State *L)
{
    zklua_handle_t *handle = luaL_checkudata(L, 1, ZKLUA_METATABLE_NAME);
}

static int zklua_adelete(lua_State *L)
{
    zklua_handle_t *handle = luaL_checkudata(L, 1, ZKLUA_METATABLE_NAME);
}

static int zklua_aexists(lua_State *L)
{
    zklua_handle_t *handle = luaL_checkudata(L, 1, ZKLUA_METATABLE_NAME);
}

static int zklua_awexists(lua_State *L)
{
    zklua_handle_t *handle = luaL_checkudata(L, 1, ZKLUA_METATABLE_NAME);
}


static int zklua_aget(lua_State *L)
{
    zklua_handle_t *handle = luaL_checkudata(L, 1, ZKLUA_METATABLE_NAME);
}

static int zklua_awget(lua_State *L)
{
    zklua_handle_t *handle = luaL_checkudata(L, 1, ZKLUA_METATABLE_NAME);
}

static int zklua_aset(lua_State *L)
{
    zklua_handle_t *handle = luaL_checkudata(L, 1, ZKLUA_METATABLE_NAME);
}

static int zklua_aget_children(lua_State *L)
{
    zklua_handle_t *handle = luaL_checkudata(L, 1, ZKLUA_METATABLE_NAME);
}

static int zklua_aget_children2(lua_State *L)
{
    zklua_handle_t *handle = luaL_checkudata(L, 1, ZKLUA_METATABLE_NAME);
}

static int zklua_awget_children(lua_State *L)
{
    zklua_handle_t *handle = luaL_checkudata(L, 1, ZKLUA_METATABLE_NAME);
}

static int zklua_awget_children2(lua_State *L)
{
    zklua_handle_t *handle = luaL_checkudata(L, 1, ZKLUA_METATABLE_NAME);
}

static int zklua_async(lua_State *L)
{
    zklua_handle_t *handle = luaL_checkudata(L, 1, ZKLUA_METATABLE_NAME);
}

static int zklua_aget_acl(lua_State *L)
{
    zklua_handle_t *handle = luaL_checkudata(L, 1, ZKLUA_METATABLE_NAME);
}

static int zklua_aset_acl(lua_State *L)
{
    zklua_handle_t *handle = luaL_checkudata(L, 1, ZKLUA_METATABLE_NAME);
}

static int zklua_error(lua_State *L)
{
    zklua_handle_t *handle = luaL_checkudata(L, 1, ZKLUA_METATABLE_NAME);
}

static int zklua_add_auth(lua_State *L)
{
    zklua_handle_t *handle = luaL_checkudata(L, 1, ZKLUA_METATABLE_NAME);
}

static int zklua_is_unrecoverable(lua_State *L)
{
    zklua_handle_t *handle = luaL_checkudata(L, 1, ZKLUA_METATABLE_NAME);
}

static int zklua_set_debug_level(lua_State *L)
{
    zklua_handle_t *handle = luaL_checkudata(L, 1, ZKLUA_METATABLE_NAME);
}

static int zklua_set_log_stream(lua_State *L)
{
    zklua_handle_t *handle = luaL_checkudata(L, 1, ZKLUA_METATABLE_NAME);
}

static int zklua_deterministic_conn_order(lua_State *L)
{
    zklua_handle_t *handle = luaL_checkudata(L, 1, ZKLUA_METATABLE_NAME);
}

static int zklua_create(lua_State *L)
{
    zklua_handle_t *handle = luaL_checkudata(L, 1, ZKLUA_METATABLE_NAME);
}

static int zklua_delete(lua_State *L)
{
    zklua_handle_t *handle = luaL_checkudata(L, 1, ZKLUA_METATABLE_NAME);
}

static int zklua_exists(lua_State *L)
{
    zklua_handle_t *handle = luaL_checkudata(L, 1, ZKLUA_METATABLE_NAME);
}

static int zklua_wexists(lua_State *L)
{
    zklua_handle_t *handle = luaL_checkudata(L, 1, ZKLUA_METATABLE_NAME);
}

static int zklua_get(lua_State *L)
{
    zklua_handle_t *handle = luaL_checkudata(L, 1, ZKLUA_METATABLE_NAME);
}

static int zklua_wget(lua_State *L)
{
    zklua_handle_t *handle = luaL_checkudata(L, 1, ZKLUA_METATABLE_NAME);
}

static int zklua_set(lua_State *L)
{
    zklua_handle_t *handle = luaL_checkudata(L, 1, ZKLUA_METATABLE_NAME);
}

static int zklua_set2(lua_State *L)
{
    zklua_handle_t *handle = luaL_checkudata(L, 1, ZKLUA_METATABLE_NAME);
}

static int zklua_get_children(lua_State *L)
{
    zklua_handle_t *handle = luaL_checkudata(L, 1, ZKLUA_METATABLE_NAME);
}

static int zklua_wget_children(lua_State *L)
{
    zklua_handle_t *handle = luaL_checkudata(L, 1, ZKLUA_METATABLE_NAME);
}

static int zklua_get_children2(lua_State *L)
{
    zklua_handle_t *handle = luaL_checkudata(L, 1, ZKLUA_METATABLE_NAME);
}

static int zklua_wget_children2(lua_State *L)
{
    zklua_handle_t *handle = luaL_checkudata(L, 1, ZKLUA_METATABLE_NAME);
}

static int zklua_get_acl(lua_State *L)
{
    zklua_handle_t *handle = luaL_checkudata(L, 1, ZKLUA_METATABLE_NAME);
}

static int zklua_set_acl(lua_State *L)
{
    zklua_handle_t *handle = luaL_checkudata(L, 1, ZKLUA_METATABLE_NAME);
}

static const luaL_Reg zklua[] =
{
    {"init", zklua_init},
    {"close", zklua_close},
    {"client_id", zklua_client_id},
    {"recv_timeout", zklua_recv_timeout},
    {"get_context", zklua_get_context},
    {"set_watcher", zklua_set_watcher},
    {"get_connected_host", zklua_get_connected_host},
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
    lua_pushliteral (L, "zklua: lua binding of apache zookeeper");
    lua_setfield(L, -2, "_DESCRIPTION");
    lua_pushliteral (L, "0.1.0");
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
