# ZKLUA: an one-minute introduction for beginners #
Zklua is the first(as far as I know) lua binding of apache zookeeper. If you are wondering what is apache zookeeper, please visit the [zookeeper official website](http://zookeeper.apache.org/ "Apache ZooKeeper") to get more information about it, hereinafter is an one-sentence description that can inform of you what is apache zookeeper: 

> [ZooKeeper](http://zookeeper.apache.org/ "Apache ZooKeeper") is a high-performance coordination service for distributed applications. It exposes common services - such as naming, configuration management, synchronization, and group services - in a simple interface so you don't have to write them from scratch. You can use it off-the-shelf to implement consensus, group management, leader election, and presence protocols.

Zklua provides a complete API binding of Lua for apache zookeeper(hereinafter referred to as `zookeeper`), including synchronous and asynchronous interface as well as some useful auxiliary APIs. That is, you may create/delete/get/update(CRUD) a zookeeper node(ZNode) in lua language synchronously and/or asynchronously without any pains.

# How to build zklua #
## Prepare to build zklua ##
First of all, you have to get zklua source code from github:

```bash
$ git clone git@github.com:forhappy/zklua.git
```

## Dependencies ##
Zklua has no other dependencies except for zookeeper c API implementation, which usually resides in `zookeeper-X.Y.Z/src/c`, hence you need to install zookeeper c API at first.

# Install zklua #

all you have to do is just a `make` to compile zklua:

```bash
$ make
```

If you want to install zklua into your system after you have compiled zklua, you need to `make install`(as root):

```bash
$ make install
```

# Getting started in 5 minutes #
## Examples ##

**Deleting a ZNode.**

Here is a tiny lua example to show you deleting a ZNode from zookeeper server.

```lua
require "zklua"
  
function zklua_my_watcher(zh, type, state, path, watcherctx)
    if type == zklua.ZOO_SESSION_EVENT then
        if state == zklua.ZOO_CONNECTED_STATE then
            print("Connected to zookeeper service successfully!\n");
            elseif (state == ZOO_EXPIRED_SESSION_STATE) then
            print("Zookeeper session expired!\n");
        end
    end
end
	
function zklua_my_void_completion(rc, data)
    print("zklua_my_void_completion:\n")
    print("rc: "..rc.."\tdata: "..data)
end
	
zklua.set_log_stream("zklua.log")
	
zh = zklua.init("127.0.0.1:2181,127.0.0.1:2182,127.0.0.1:2183", zklua_my_watcher, 10000)
	
ret = zklua.adelete(zh, "/zklua", -1, zklua_my_void_completion, "zklua adelete.")
print("zklua.adelete ret: "..ret)
	
print("hit any key to continue...")
io.read()
```

**Set watch on a specified path.**

Here is a tiny lua example to show you set a watch on a specified path.

```lua
require "zklua"
    
function zklua_my_global_watcher(zh, type, state, path, watcherctx)
    if type == zklua.ZOO_SESSION_EVENT then
        if state == zklua.ZOO_CONNECTED_STATE then
            print("Connected to zookeeper service successfully!\n");
         elseif (state == ZOO_EXPIRED_SESSION_STATE) then
            print("Zookeeper session expired!\n");
        end
    end
end

function zklua_my_local_watcher(zh, type, state, path, watcherctx)
    print("zklua_my_local_watcher(".."type: "..type..", state: "..state..", path: "..path..")")
    print("zklua_my_local_watcher(".."watcherctx: "..watcherctx..")")
end
    
function zklua_my_stat_completion(rc, stat, data)
    print("zklua_my_stat_completion:\n")
    print("rc: "..rc.."\tdata: "..data)
end
    
zklua.set_log_stream("zklua.log")
    
zh = zklua.init("127.0.0.1:2181,127.0.0.1:2182,127.0.0.1:2183", zklua_my_global_watcher, 10000)
    
ret = zklua.awexists(zh, "/zklua",
                     zklua_my_local_watcher,"zklua_my_local_watcher",
                     zklua_my_stat_completion, "zklua aexists.")
print("zklua.aexists ret: "..ret)
    
print("hit any key to continue...")
io.read()
```

# API specification #
See [docs/zklua.lua](https://raw.githubusercontent.com/forhappy/zklua/master/docs/zklua.lua) for more details about zklua's API specification.

# License #
Licensed to the Apache Software Foundation (ASF) under one
or more contributor license agreements. See the NOTICE file
distributed with this work for additional information
regarding copyright ownership. The ASF licenses this file
to you under the Apache License, Version 2.0 (the
"License"); you may not use this file except in compliance
with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
