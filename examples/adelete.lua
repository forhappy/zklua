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
