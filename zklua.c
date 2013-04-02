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

#include "zklua.h"

static int zklua_init(lua_State *L)
{}

static int zklua_close(lua_State *L)
{}

static int zklua_client_id(lua_State *L)
{}

static int zklua_recv_timeout(lua_State *L)
{}

static int zklua_get_context(lua_State *L)
{}

static int zklua_set_context(lua_State *L)
{}

static int zklua_set_watcher(lua_State *L)
{}

static int zklua_get_connected_host(lua_State *L)
{}

static int zklua_interest(lua_State *L)
{}

static int zklua_process(lua_State *L)
{}

static int zklua_state(lua_State *L)
{}

static int zklua_acreate(lua_State *L)
{}

static int zklua_adelete(lua_State *L)
{}

static int zklua_aexists(lua_State *L)
{}

static int zklua_awexists(lua_State *L)
{}


static int zklua_aget(lua_State *L)
{}

static int zklua_awget(lua_State *L)
{}

static int zklua_aset(lua_State *L)
{}

static int zklua_aget_children(lua_State *L)
{}

static int zklua_aget_children2(lua_State *L)
{}

static int zklua_awget_children(lua_State *L)
{}

static int zklua_awget_children2(lua_State *L)
{}

static int zklua_async(lua_State *L)
{}

static int zklua_aget_acl(lua_State *L)
{}

static int zklua_aset_acl(lua_State *L)
{}

static int zklua_error(lua_State *L)
{}

static int zklua_add_auth(lua_State *L)
{}

static int zklua_is_unrecoverable(lua_State *L)
{}

static int zklua_set_debug_level(lua_State *L)
{}

static int zklua_set_log_stream(lua_State *L)
{}

static int zklua_deterministic_conn_order(lua_State *L)
{}

static int zklua_create(lua_State *L)
{}

static int zklua_delete(lua_State *L)
{}

static int zklua_exists(lua_State *L)
{}

static int zklua_wexists(lua_State *L)
{}

static int zklua_get(lua_State *L)
{}

static int zklua_wget(lua_State *L)
{}

static int zklua_set(lua_State *L)
{}

static int zklua_set2(lua_State *L)
{}

static int zklua_get_children(lua_State *L)
{}

static int zklua_wget_children(lua_State *L)
{}

static int zklua_get_children2(lua_State *L)
{}

static int zklua_wget_children2(lua_State *L)
{}

static int zklua_get_acl(lua_State *L)
{}

static int zklua_set_acl(lua_State *L)
{}

static int zklua_(lua_State *L)
{}

static const luaL_Reg zklua[] =
{
    {"acreate", zklua_acreate},
    {NULL, NULL}
};

int luaopen_zklua(lua_State *L)
{
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
    return 1;
}
