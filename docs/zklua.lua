---Lua binding of Apache Zookeeper.

---signature of a watcher function.
--
--There are two ways to receive watch notifications: legacy and watcher object.<p>
--The legacy style, an application wishing to receive events from ZooKeeper must
--first implement a function with this signature and pass a pointer to the function
--to @see zookeeper_init. Next, the application sets a watch by calling one of
--the getter API that accept the watch integer flag (
--for example, @see zoo_aexists, @see zoo_get, etc).
--
--The watcher object style uses an instance of a "watcher object" which in
--the C world is represented by a pair: a pointer to a function implementing this
--signature and a pointer to watcher context -- handback user-specific data.
--When a watch is triggered this function will be called along with
--the watcher context. An application wishing to use this style must use
--the getter API functions with the "w" prefix in their names (
--for example, @see zoo_awexists, @see zoo_wget, etc).
--
--@param zh zookeeper handle.
--@param type event type. This is one of the *_EVENT constants.
--@param state connection state. The state value will be one of the *_STATE constants.
--@param path znode path for which the watcher is triggered. NULL if the event type is ZOO_SESSION_EVENT.
--@param watcherctx watcher context.
function wather_fn(zh, type, state, path, watcherctx) end


---signature of a completion function for a call that returns void.
--
--This method will be invoked at the end of a asynchronous call and also as
--a result of connection loss or timeout.
--@param rc the error code of the call. Connection loss/timeout triggers
--the completion with one of the following error codes:<br>
--ZCONNECTIONLOSS -- lost connection to the server<br>
--ZOPERATIONTIMEOUT -- connection timed out<br>
--Data related events trigger the completion with error codes listed the
--Exceptions section of the documentation of the function that initiated the
--call. (Zero indicates call was successful.)
--@param data the pointer that was passed by the caller when the function
--that this completion corresponds to was invoked. The programmer
--is responsible for any memory freeing associated with the data pointer.
function void_completion(rc, data) end


---signature of a completion function that returns a Stat structure.
--
--This method will be invoked at the end of a asynchronous call and also as
--a result of connection loss or timeout.
--@param rc the error code of the call. Connection loss/timeout triggers
--the completion with one of the following error codes:<br>
--ZCONNECTIONLOSS -- lost connection to the server<br>
--ZOPERATIONTIMEOUT -- connection timed out<br>
--Data related events trigger the completion with error codes listed the
--Exceptions section of the documentation of the function that initiated the
--call. (Zero indicates call was successful.)
--@param stat a pointer to the stat information for the node involved
--in this function. If a non zero error code is returned, the content of stat
--is undefined. The programmer is NOT responsible for freeing stat.
--@param data the pointer that was passed by the caller when the function
--that this completion corresponds to was invoked. The programmer
--is responsible for any memory freeing associated with the data pointer.
function stat_completion(rc, stat, data) end


---signature of a completion function that returns data.
--
--This method will be invoked at the end of a asynchronous call and also as
--a result of connection loss or timeout.
--@param rc the error code of the call. Connection loss/timeout triggers
--the completion with one of the following error codes:<br>
--ZCONNECTIONLOSS -- lost connection to the server<br>
--ZOPERATIONTIMEOUT -- connection timed out<br>
--Data related events trigger the completion with error codes listed the
--Exceptions section of the documentation of the function that initiated the
--call. (Zero indicates call was successful.)
--@param value the value of the information returned by the asynchronous call.
--If a non zero error code is returned, the content of value is undefined.
--The programmer is NOT responsible for freeing value.
--@param value_len the number of bytes in value.
--@param stat a pointer to the stat information for the node involved in
--this function. If a non zero error code is returned, the content of
--stat is undefined. The programmer is NOT responsible for freeing stat.
--@param data the pointer that was passed by the caller when the function
--that this completion corresponds to was invoked. The programmer
--is responsible for any memory freeing associated with the data pointer.
function data_completion(rc, value, value_len, stat, data) end


---signature of a completion function that returns a list of strings.
--
--This method will be invoked at the end of a asynchronous call and also as
--a result of connection loss or timeout.
--@param rc the error code of the call. Connection loss/timeout triggers
--the completion with one of the following error codes:<br>
--ZCONNECTIONLOSS -- lost connection to the server<br>
--ZOPERATIONTIMEOUT -- connection timed out<br>
--Data related events trigger the completion with error codes listed the
--Exceptions section of the documentation of the function that initiated the
--call. (Zero indicates call was successful.)
--@param strings a pointer to the structure containng the list of strings of the
--names of the children of a node. If a non zero error code is returned,
--the content of strings is undefined. The programmer is NOT responsible
--for freeing strings.
--@param data the pointer that was passed by the caller when the function
--that this completion corresponds to was invoked. The programmer
--is responsible for any memory freeing associated with the data pointer.
function strings_completion(rc, strings, data) end


---signature of a completion function that returns a list of strings and stat.
--
--This method will be invoked at the end of a asynchronous call and also as
--a result of connection loss or timeout.
--@param rc the error code of the call. Connection loss/timeout triggers
--the completion with one of the following error codes:<br>
--ZCONNECTIONLOSS -- lost connection to the server<br>
--ZOPERATIONTIMEOUT -- connection timed out<br>
--Data related events trigger the completion with error codes listed the
--Exceptions section of the documentation of the function that initiated the
--call. (Zero indicates call was successful.)
--@param strings a pointer to the structure containng the list of strings of the
--names of the children of a node. If a non zero error code is returned,
--the content of strings is undefined. The programmer is NOT responsible
--for freeing strings.
--@param stat a pointer to the stat information for the node involved in
--this function. If a non zero error code is returned, the content of
--stat is undefined. The programmer is NOT responsible for freeing stat.
--@param data the pointer that was passed by the caller when the function
--that this completion corresponds to was invoked. The programmer
--is responsible for any memory freeing associated with the data
--pointer.
function strings_stat_completion(rc, strings, stat, data) end


---signature of a completion function that returns a list of strings.
--
--This method will be invoked at the end of a asynchronous call and also as
--a result of connection loss or timeout.
--@param rc the error code of the call. Connection loss/timeout triggers
--the completion with one of the following error codes:<br>
--ZCONNECTIONLOSS -- lost connection to the server<br>
--ZOPERATIONTIMEOUT -- connection timed out<br>
--Data related events trigger the completion with error codes listed the
--Exceptions section of the documentation of the function that initiated the
--call. (Zero indicates call was successful.)
--@param value the value of the string returned.
--@param data the pointer that was passed by the caller when the function
--that this completion corresponds to was invoked. The programmer
--is responsible for any memory freeing associated with the data pointer.
function string_completion(rc, value, data) end


---signature of a completion function that returns an ACL.
--
--This method will be invoked at the end of a asynchronous call and also as
--a result of connection loss or timeout.
--@param rc the error code of the call. Connection loss/timeout triggers
--the completion with one of the following error codes:<br>
--ZCONNECTIONLOSS -- lost connection to the server<br>
--ZOPERATIONTIMEOUT -- connection timed out<br>
--Data related events trigger the completion with error codes listed the
--Exceptions section of the documentation of the function that initiated the
--call. (Zero indicates call was successful.)
--@param acl a pointer to the structure containng the ACL of a node. If a non
--zero error code is returned, the content of strings is undefined. The
--programmer is NOT responsible for freeing acl.
--@param stat a pointer to the stat information for the node involved in
--this function. If a non zero error code is returned, the content of
--stat is undefined. The programmer is NOT responsible for freeing stat.
--@param data the pointer that was passed by the caller when the function
--that this completion corresponds to was invoked. The programmer
--is responsible for any memory freeing associated with the data pointer.
function acl_completion(rc, acl, stat, data) end


---create a handle to used communicate with zookeeper.
--
--This method creates a new handle and a zookeeper session that corresponds
--to that handle. Session establishment is asynchronous, meaning that the
--session should not be considered established until (and unless) an
--event of state ZOO_CONNECTED_STATE is received.
--
--@param host comma separated host:port pairs, each corresponding to a zk server.
--e.g. "127.0.0.1:3000,127.0.0.1:3001,127.0.0.1:3002".
--@param watcher_fn the global watcher callback function. When notifications are
--triggered this function will be invoked.
--@param recv_timeout the recv timeout.
--@param clientid the id of a previously established session that this
--client will be reconnecting to. Pass 0 if not reconnecting to a previous
--session. Clients can access the session id of an established, valid,
--connection by calling \ref zoo_client_id. If the session corresponding to
--the specified clientid has expired, or if the clientid is invalid for
--any reason, the returned zhandle_t will be invalid -- the zhandle_t
--state will indicate the reason for failure (typically ZOO_EXPIRED_SESSION_STATE).
--@param context the handback object that will be associated with this instance
--of zhandle_t. Application can access it (for example, in the watcher callback)
--using @see zoo_get_context. The object is not used by zookeeper internally and can be null.
--@param flags reserved for future use. Should be set to zero.
--@return a pointer to the opaque zhandle structure. If it fails to create a new zhandle the function returns NULL and the errno variable indicates the reason.
function init(host, watcher_fn, recv_timeout, clientid, context, flags) end


---close the zookeeper handle and free up any resources.
--
--After this call, the client session will no longer be valid. The function
--will flush any outstanding send requests before return. As a result it may block.
--
--This method should only be called only once on a zookeeper handle. Calling
--twice will cause undefined (and probably undesirable behavior). Calling any other
--zookeeper method after calling close is undefined behaviour and should be avoided.
--
--@param zh the zookeeper handle obtained by a call to @see init
--@return a result code, regardless of the error code returned, the zhandle will be destroyed and all resources freed.
--
--ZOK - success <br>
--ZBADARGUMENTS - invalid input parameters <br>
--ZMARSHALLINGERROR - failed to marshall a request; possibly, out of memory.<br>
--ZOPERATIONTIMEOUT - failed to flush the buffers within the specified timeout.<br>
--ZCONNECTIONLOSS - a network error occured while attempting to send request to server.<br>
--ZSYSTEMERROR -- a system (OS) error occured; it's worth checking errno to get details.<br>
function close(zh) end
