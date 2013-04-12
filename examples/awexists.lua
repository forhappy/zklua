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

ret = zklua.awexists(zh, "/zklua", zklua_my_local_watcher,"zklua_my_local_watcher", zklua_my_stat_completion, "zklua aexists.")
print("zklua.aexists ret: "..ret)

print("hit any key to continue...")
io.read()
