/*
 * ========================================================================
 *
 *       Filename:  zklua.h
 *
 *    Description:  lua binding of apache zookeeper.
 *
 *        Created:  04/02/2013 10:35:12 PM
 *
 *         Author:  Fu Haiping (forhappy), haipingf@gmail.com
 *        Company:  ICT ( Institute Of Computing Technology, CAS )
 *
 * ========================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include <zookeeper/zookeeper.h>

typedef struct zklua_completion_data_s zklua_completion_data_t;
typedef struct zklua_handle_s zklua_handle_t;

struct zklua_handle_s {
    zhandle_t *zh;
};

struct zklua_completion_data_s {
    lua_State *L;
    const void *data;
};

void void_completion_dispatch(int rc, const void *data);

void stat_completion_dispatch(int rc, const struct Stat *stat,
        const void *data);

void data_completion_dispatch(int rc, const char *value, int value_len,
        const struct Stat *stat, const void *data);

void strings_completion_dispatch(int rc, const struct String_vector *strings,
        const void *data);

void strings_stat_completion_t(int rc, const struct String_vector *strings,
        const struct Stat *stat, const void *data);

void string_completion_t(int rc, const char *value, const void *data);

void acl_completion_t(int rc, struct ACL_vector *acl,
        struct Stat *stat, const void *data);

