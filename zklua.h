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

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include <zookeeper/zookeeper.h>

#define ZKLUA_METATABLE_NAME "ZKLUA_HANDLE"
#define ZKLUA_MAX_PATH_BUFFER_SIZE 1024

typedef struct zklua_handle_s zklua_handle_t;
typedef struct zklua_global_watcher_context_s zklua_global_watcher_context_t;
typedef struct zklua_local_watcher_context_s zklua_local_watcher_context_t;
typedef struct zklua_completion_data_s zklua_completion_data_t;

struct zklua_handle_s {
    zhandle_t *zh;
};

struct zklua_global_watcher_context_s {
    lua_State *L;
    void *context;
};

struct zklua_local_watcher_context_s {
    lua_State *L;
    void *context;
    int zhref;
    int cbref;
};

struct zklua_completion_data_s {
    lua_State *L;
    const void *data;
};

void watcher_dispatch(zhandle_t *zh, int type, int state,
        const char *path,void *watcherCtx);

void void_completion_dispatch(int rc, const void *data);

void stat_completion_dispatch(int rc, const struct Stat *stat,
        const void *data);

void data_completion_dispatch(int rc, const char *value, int value_len,
        const struct Stat *stat, const void *data);

void strings_completion_dispatch(int rc, const struct String_vector *strings,
        const void *data);

void strings_stat_completion_dispatch(int rc, const struct String_vector *strings,
        const struct Stat *stat, const void *data);

void string_completion_dispatch(int rc, const char *value, const void *data);

void acl_completion_dispatch(int rc, struct ACL_vector *acl,
        struct Stat *stat, const void *data);

