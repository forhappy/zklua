---Lua binding of Apache Zookeeper.

---signature of a watcher function.
--
--There are two ways to receive watch notifications: legacy and watcher object.
--The legacy style, an application wishing to receive events from ZooKeeper must
--first implement a function with this signature and pass a pointer to the
--function to init.
--
--Next, the application sets a watch by calling one of the getter API
--that accept the watch integer flag (for example,  aexists,  get, etc).
--
--The watcher object style uses an instance of a "watcher object" which in
--the C world is represented by a pair: a pointer to a function implementing this
--signature and a pointer to watcher context -- handback user-specific data.
--When a watch is triggered this function will be called along with
--the watcher context. An application wishing to use this style must use
--the getter API functions with the "w" prefix in their names (
--for example,  awexists,  wget, etc).
--
--@param zh zookeeper handle.
--@param type event type. This is one of the *_EVENT constants.
--@param state connection state. The state value will be one of the *_STATE constants.
--@param path znode path for which the watcher is triggered. NULL if the event type is ZOO_SESSION_EVENT.
--@param watcherctx watcher context.
function wather_fn(zh, type, state, path, watcherctx) end


---signature of a void completion function.
--
--This method will be invoked at the end of a asynchronous call and also as
--a result of connection loss or timeout.
--@param rc the error code of the call. Connection loss/timeout triggers
--the completion with one of the following error codes:
--ZCONNECTIONLOSS -- lost connection to the server
--ZOPERATIONTIMEOUT -- connection timed out
--Data related events trigger the completion with error codes listed the
--Exceptions section of the documentation of the function that initiated the
--call. (Zero indicates call was successful.)
--@param data the pointer that was passed by the caller when the function
--that this completion corresponds to was invoked. The programmer
--is responsible for any memory freeing associated with the data pointer.
function void_completion(rc, data) end


---signature of a stat completion function.
--
--This method will be invoked at the end of a asynchronous call and also as
--a result of connection loss or timeout.
--@param rc the error code of the call. Connection loss/timeout triggers
--the completion with one of the following error codes:
--ZCONNECTIONLOSS -- lost connection to the server
--ZOPERATIONTIMEOUT -- connection timed out
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


---signature of a data completion function.
--
--This method will be invoked at the end of a asynchronous call and also as
--a result of connection loss or timeout.
--@param rc the error code of the call. Connection loss/timeout triggers
--the completion with one of the following error codes:
--ZCONNECTIONLOSS -- lost connection to the server
--ZOPERATIONTIMEOUT -- connection timed out
--Data related events trigger the completion with error codes listed the
--Exceptions section of the documentation of the function that initiated the
--call. (Zero indicates call was successful.)
--@param value the value of the information returned by the asynchronous call.
--If a non zero error code is returned, the content of value is undefined.
--The programmer is NOT responsible for freeing value.
--@param stat a pointer to the stat information for the node involved in
--this function. If a non zero error code is returned, the content of
--stat is undefined. The programmer is NOT responsible for freeing stat.
--@param data the pointer that was passed by the caller when the function
--that this completion corresponds to was invoked. The programmer
--is responsible for any memory freeing associated with the data pointer.
function data_completion(rc, value, stat, data) end


---signature of a strings completion function.
--
--This method will be invoked at the end of a asynchronous call and also as
--a result of connection loss or timeout.
--@param rc the error code of the call. Connection loss/timeout triggers
--the completion with one of the following error codes:
--ZCONNECTIONLOSS -- lost connection to the server
--ZOPERATIONTIMEOUT -- connection timed out
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


---signature of a strings and stat completion function.
--
--This method will be invoked at the end of a asynchronous call and also as
--a result of connection loss or timeout.
--@param rc the error code of the call. Connection loss/timeout triggers
--the completion with one of the following error codes:
--ZCONNECTIONLOSS -- lost connection to the server
--ZOPERATIONTIMEOUT -- connection timed out
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


---signature of a string completion function.
--
--This method will be invoked at the end of a asynchronous call and also as
--a result of connection loss or timeout.
--@param rc the error code of the call. Connection loss/timeout triggers
--the completion with one of the following error codes:
--ZCONNECTIONLOSS -- lost connection to the server
--ZOPERATIONTIMEOUT -- connection timed out
--Data related events trigger the completion with error codes listed the
--Exceptions section of the documentation of the function that initiated the
--call. (Zero indicates call was successful.)
--@param value the value of the string returned.
--@param data the pointer that was passed by the caller when the function
--that this completion corresponds to was invoked. The programmer
--is responsible for any memory freeing associated with the data pointer.
function string_completion(rc, value, data) end


---signature of a ACL completion function.
--
--This method will be invoked at the end of a asynchronous call and also as
--a result of connection loss or timeout.
--@param rc the error code of the call. Connection loss/timeout triggers
--the completion with one of the following error codes:
--ZCONNECTIONLOSS -- lost connection to the server
--ZOPERATIONTIMEOUT -- connection timed out
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


---create a zookeeper handle.
--The handle is used to communicate with zookeeper.
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
--connection by calling  client_id. If the session corresponding to
--the specified clientid has expired, or if the clientid is invalid for
--any reason, the returned zhandle_t will be invalid -- the zhandle_t
--state will indicate the reason for failure (typically ZOO_EXPIRED_SESSION_STATE).
--@param context the handback object that will be associated with this instance
--of zhandle_t. Application can access it (for example, in the watcher callback)
--using  get_context. The object is not used by zookeeper internally and can be null.
--@param flags reserved for future use. Should be set to zero.
--@return a pointer to the opaque zhandle structure. If it fails to create a new zhandle the function returns NULL and the errno variable indicates the reason.
function init(host, watcher_fn, recv_timeout, clientid, context, flags) end


---close the zookeeper handle.
--
--After this call, the client session will no longer be valid. The function
--will flush any outstanding send requests before return. As a result it may block.
--
--This method should only be called only once on a zookeeper handle. Calling
--twice will cause undefined (and probably undesirable behavior). Calling any other
--zookeeper method after calling close is undefined behaviour and should be avoided.
--
--@param zh the zookeeper handle obtained by a call to  init
--@return a result code, regardless of the error code returned, the zhandle will be destroyed and all resources freed.
--
--ZOK - success
--ZBADARGUMENTS - invalid input parameters
--ZMARSHALLINGERROR - failed to marshall a request; possibly, out of memory.
--ZOPERATIONTIMEOUT - failed to flush the buffers within the specified timeout.
--ZCONNECTIONLOSS - a network error occured while attempting to send request to server.
--ZSYSTEMERROR -- a system (OS) error occured; it's worth checking errno to get details.
function close(zh) end


---return the client session id.
--only valid if the connections is currently connected (ie. last watcher state is ZOO_CONNECTED_STATE).
function client_id(zh) end


---return the timeout for this session.
--only valid if the connections is currently connected (ie. last watcher state is ZOO_CONNECTED_STATE).
--This value may change after a server re-connect.
function recv_timeout(zh) end


---return the context for this handle.
function get_context(zh) end

---set the context for this handle.
function set_context(zh, context) end

---set a watcher function.
--@return previous watcher function.
function set_watcher(zh, newfn) end


---return the socket address.
--only valid if the current connection is alive.
--@return socket address of the connected host or NULL on failure, only valid if the connection is current connected.
function get_connected_host(zh) end

--
---create a node.
--
--This method will create a node in ZooKeeper. A node can only be created if
--it does not already exists. The Create Flags affect the creation of nodes.
--If ZOO_EPHEMERAL flag is set, the node will automatically get removed if the
--client session goes away. If the ZOO_SEQUENCE flag is set, a unique
--monotonically increasing sequence number is appended to the path name. The
--sequence number is always fixed length of 10 digits, 0 padded.
--
--@param zh the zookeeper handle obtained by a call to  init
--@param path The name of the node. Expressed as a file name with slashes
--separating ancestors of the node.
--@param value The data to be stored in the node.
--@param acl The initial ACL of the node. The ACL must not be null or empty.
--@param flags this parameter can be set to 0 for normal create or an OR
--of the Create Flags.
--@param string_completion the routine to invoke when the request completes. The completion
--will be triggered with one of the following codes passed in as the rc argument:
--ZOK operation completed successfully
--ZNONODE the parent node does not exist.
--ZNODEEXISTS the node already exists
--ZNOAUTH the client does not have permission.
--ZNOCHILDRENFOREPHEMERALS cannot create children of ephemeral nodes.
--@param data The data that will be passed to the completion routine when the
--function completes.
--@return ZOK on success or one of the following errcodes on failure:
--ZBADARGUMENTS - invalid input parameters
--ZINVALIDSTATE - zhandle state is either ZOO_SESSION_EXPIRED_STATE or ZOO_AUTH_FAILED_STATE
--ZMARSHALLINGERROR - failed to marshall a request; possibly, out of memory
--
function acreate(zh, path, value, acl, flags, string_completion, data) end


---delete a node in zookeeper.
--
--@param zh the zookeeper handle obtained by a call to  init
--@param path the name of the node. Expressed as a file name with slashes 
--separating ancestors of the node.
--@param version the expected version of the node. The function will fail if the
--actual version of the node does not match the expected version.
--If -1 is used the version check will not take place. 
--@param void_completion the routine to invoke when the request completes. The completion
--will be triggered with one of the following codes passed in as the rc argument:
--ZOK operation completed successfully
--ZNONODE the node does not exist.
--ZNOAUTH the client does not have permission.
--ZBADVERSION expected version does not match actual version.
--ZNOTEMPTY children are present; node cannot be deleted.
--@param data the data that will be passed to the completion routine when 
--the function completes.
--@return ZOK on success or one of the following errcodes on failure:
--ZBADARGUMENTS - invalid input parameters
--ZINVALIDSTATE - zhandle state is either ZOO_SESSION_EXPIRED_STATE or ZOO_AUTH_FAILED_STATE
--ZMARSHALLINGERROR - failed to marshall a request; possibly, out of memory
--
function adelete(zh, path, version, void_completion, data) end


---checks the existence of a node in zookeeper.
--
--@param zh the zookeeper handle obtained by a call to  init
--@param path the name of the node. Expressed as a file name with slashes 
--separating ancestors of the node.
--@param watch if nonzero, a watch will be set at the server to notify the 
--client if the node changes. The watch will be set even if the node does not 
--exist. This allows clients to watch for nodes to appear.
--@param stat_completion the routine to invoke when the request completes. The completion
--will be triggered with one of the following codes passed in as the rc argument:
--ZOK operation completed successfully
--ZNONODE the node does not exist.
--ZNOAUTH the client does not have permission.
--@param data the data that will be passed to the completion routine when the 
--function completes.
--@return ZOK on success or one of the following errcodes on failure:
--ZBADARGUMENTS - invalid input parameters
--ZINVALIDSTATE - zhandle state is either ZOO_SESSION_EXPIRED_STATE or ZOO_AUTH_FAILED_STATE
--ZMARSHALLINGERROR - failed to marshall a request; possibly, out of memory
--
function aexists(zh, path, watch, stat_completion, data) end


--- checks the existence of a node in zookeeper.
--
--This function is similar to  axists except it allows one specify 
--a watcher object - a function pointer and associated context. The function
--will be called once the watch has fired. The associated context data will be 
--passed to the function as the watcher context parameter. 
--
--@param zh the zookeeper handle obtained by a call to  init
--@param path the name of the node. Expressed as a file name with slashes 
--separating ancestors of the node.
--@param watcher_fn if non-null a watch will set on the specified znode on the server.
--The watch will be set even if the node does not exist. This allows clients 
--to watch for nodes to appear.
--@param watcherctx user specific data, will be passed to the watcher callback.
--Unlike the global context set by  init, this watcher context
--is associated with the given instance of the watcher only.
--@param stat_completion the routine to invoke when the request completes. The completion
--will be triggered with one of the following codes passed in as the rc argument:
--ZOK operation completed successfully
--ZNONODE the node does not exist.
--ZNOAUTH the client does not have permission.
--@param data the data that will be passed to the completion routine when the 
--function completes.
--@return ZOK on success or one of the following errcodes on failure:
--ZBADARGUMENTS - invalid input parameters
--ZINVALIDSTATE - zhandle state is either ZOO_SESSION_EXPIRED_STATE or ZOO_AUTH_FAILED_STATE
--ZMARSHALLINGERROR - failed to marshall a request; possibly, out of memory
--
function awexists(zh, path, watcher_fn, watcherctx, stat_completion, data) end


---gets the data associated with a node.
--
--@param zh the zookeeper handle obtained by a call to  init
--@param path the name of the node. Expressed as a file name with slashes 
--separating ancestors of the node.
--@param watch if nonzero, a watch will be set at the server to notify 
--the client if the node changes.
--@param data_completion the routine to invoke when the request completes. The completion
--will be triggered with one of the following codes passed in as the rc argument:
--ZOK operation completed successfully
--ZNONODE the node does not exist.
--ZNOAUTH the client does not have permission.
--@param data the data that will be passed to the completion routine when 
--the function completes.
--@return ZOK on success or one of the following errcodes on failure:
--ZBADARGUMENTS - invalid input parameters
--ZINVALIDSTATE - zhandle state is either in ZOO_SESSION_EXPIRED_STATE or ZOO_AUTH_FAILED_STATE
--ZMARSHALLINGERROR - failed to marshall a request; possibly, out of memory
--
function aget(zh, path, watch, data_completion, data) end


---gets the data associated with a node.
--
--This function is similar to  aget except it allows one specify
--a watcher object rather than a boolean watch flag.
--
--@param zh the zookeeper handle obtained by a call to  init
--@param path the name of the node. Expressed as a file name with slashes
--separating ancestors of the node.
--@param watcher_fn if non-null, a watch will be set at the server to notify
--the client if the node changes.
--@param watcherctx user specific data, will be passed to the watcher callback.
--Unlike the global context set by  init, this watcher context
--is associated with the given instance of the watcher only.
--@param data_completion the routine to invoke when the request completes. The completion
--will be triggered with one of the following codes passed in as the rc argument:
--ZOK operation completed successfully
--ZNONODE the node does not exist.
--ZNOAUTH the client does not have permission.
--@param data the data that will be passed to the completion routine when
--the function completes.
--@return ZOK on success or one of the following errcodes on failure:
--ZBADARGUMENTS - invalid input parameters
--ZINVALIDSTATE - zhandle state is either in ZOO_SESSION_EXPIRED_STATE or ZOO_AUTH_FAILED_STATE
--ZMARSHALLINGERROR - failed to marshall a request; possibly, out of memory
--
function awget(zh, path, watcher_fn, watcherctx, data_completion, data) end


---sets the data associated with a node.
--
--@param zh the zookeeper handle obtained by a call to  init
--@param path the name of the node. Expressed as a file name with slashes 
--separating ancestors of the node.
--@param buffer the buffer holding data to be written to the node.
--@param version the expected version of the node. The function will fail if 
--the actual version of the node does not match the expected version. If -1 is 
--used the version check will not take place. * completion: If null, 
--the function will execute synchronously. Otherwise, the function will return 
--immediately and invoke the completion routine when the request completes.
--@param stat_completion the routine to invoke when the request completes. The completion
--will be triggered with one of the following codes passed in as the rc argument:
--ZOK operation completed successfully
--ZNONODE the node does not exist.
--ZNOAUTH the client does not have permission.
--ZBADVERSION expected version does not match actual version.
--@param data the data that will be passed to the completion routine when 
--the function completes.
--@return ZOK on success or one of the following errcodes on failure:
--ZBADARGUMENTS - invalid input parameters
--ZINVALIDSTATE - zhandle state is either ZOO_SESSION_EXPIRED_STATE or ZOO_AUTH_FAILED_STATE
--ZMARSHALLINGERROR - failed to marshall a request; possibly, out of memory
--
function aset(zh, path, buffer, version, stat_completion, data) end


---lists the children of a node.
--
--@param zh the zookeeper handle obtained by a call to  init
--@param path the name of the node. Expressed as a file name with slashes
--separating ancestors of the node.
--@param watch if nonzero, a watch will be set at the server to notify
--the client if the node changes.
--@param strings_completion the routine to invoke when the request completes. The completion
--will be triggered with one of the following codes passed in as the rc argument:
--ZOK operation completed successfully
--ZNONODE the node does not exist.
--ZNOAUTH the client does not have permission.
--@param data the data that will be passed to the completion routine when
--the function completes.
--@return ZOK on success or one of the following errcodes on failure:
--ZBADARGUMENTS - invalid input parameters
--ZINVALIDSTATE - zhandle state is either ZOO_SESSION_EXPIRED_STATE or ZOO_AUTH_FAILED_STATE
--ZMARSHALLINGERROR - failed to marshall a request; possibly, out of memory
--
function aget_children(zh, path, watch, strings_completion, data) end


---lists the children of a node.
--
--This function is similar to  aget_children except it allows one specify
--a watcher object rather than a boolean watch flag.
--
--@param zh the zookeeper handle obtained by a call to  init
--@param path the name of the node. Expressed as a file name with slashes
--separating ancestors of the node.
--@param watcher_fn if non-null, a watch will be set at the server to notify
--the client if the node changes.
--@param watcherctx user specific data, will be passed to the watcher callback.
--Unlike the global context set by  init, this watcher context
--is associated with the given instance of the watcher only.
--@param strings_completion the routine to invoke when the request completes. The completion
--will be triggered with one of the following codes passed in as the rc argument:
--ZOK operation completed successfully
--ZNONODE the node does not exist.
--ZNOAUTH the client does not have permission.
--@param data the data that will be passed to the completion routine when
--the function completes.
--@return ZOK on success or one of the following errcodes on failure:
--ZBADARGUMENTS - invalid input parameters
--ZINVALIDSTATE - zhandle state is either ZOO_SESSION_EXPIRED_STATE or ZOO_AUTH_FAILED_STATE
--ZMARSHALLINGERROR - failed to marshall a request; possibly, out of memory
--
function awget_children(zh, path, watcher_fn, watcherctx, strings_completion, data) end
--
--
---lists the children of a node, and get the parent stat.
--
--This function is new in version 3.3.0
--
--@param zh the zookeeper handle obtained by a call to  init
--@param path the name of the node. Expressed as a file name with slashes
--separating ancestors of the node.
--@param watch if nonzero, a watch will be set at the server to notify
--the client if the node changes.
--@param string_stat_completion the routine to invoke when the request completes. The completion
--will be triggered with one of the following codes passed in as the rc argument:
--ZOK operation completed successfully
--ZNONODE the node does not exist.
--ZNOAUTH the client does not have permission.
--@param data the data that will be passed to the completion routine when
--the function completes.
--@return ZOK on success or one of the following errcodes on failure:
--ZBADARGUMENTS - invalid input parameters
--ZINVALIDSTATE - zhandle state is either ZOO_SESSION_EXPIRED_STATE or ZOO_AUTH_FAILED_STATE
--ZMARSHALLINGERROR - failed to marshall a request; possibly, out of memory
--
function aget_children2(zh, path, watch, strings_stat_completion, data) end


---lists the children of a node, and get the parent stat.
--
--This function is similar to  aget_children2 except it allows one specify
--a watcher object rather than a boolean watch flag.
--
--This function is new in version 3.3.0
--
--@param zh the zookeeper handle obtained by a call to  init
--@param path the name of the node. Expressed as a file name with slashes
--separating ancestors of the node.
--@param watcher_fn if non-null, a watch will be set at the server to notify
--the client if the node changes.
--@param watcherctx user specific data, will be passed to the watcher callback.
--Unlike the global context set by  init, this watcher context
--is associated with the given instance of the watcher only.
--@param strings_stat_completion the routine to invoke when the request completes. The completion
--will be triggered with one of the following codes passed in as the rc argument:
--ZOK operation completed successfully
--ZNONODE the node does not exist.
--ZNOAUTH the client does not have permission.
--@param data the data that will be passed to the completion routine when
--the function completes.
--@return ZOK on success or one of the following errcodes on failure:
--ZBADARGUMENTS - invalid input parameters
--ZINVALIDSTATE - zhandle state is either ZOO_SESSION_EXPIRED_STATE or ZOO_AUTH_FAILED_STATE
--ZMARSHALLINGERROR - failed to marshall a request; possibly, out of memory
--
function awget_children2(zh, path, watcher_fn, watcherctx, strings_stat_completion, data) end


---Flush leader channel.
--
--@param zh the zookeeper handle obtained by a call to  init
--@param path the name of the node. Expressed as a file name with slashes
--separating ancestors of the node.
--@param string_completion the routine to invoke when the request completes. The completion
--will be triggered with one of the following codes passed in as the rc argument:
--ZOK operation completed successfully
--ZNONODE the node does not exist.
--ZNOAUTH the client does not have permission.
--@param data the data that will be passed to the completion routine when
--the function completes.
--@return ZOK on success or one of the following errcodes on failure:
--ZBADARGUMENTS - invalid input parameters
--ZINVALIDSTATE - zhandle state is either ZOO_SESSION_EXPIRED_STATE or ZOO_AUTH_FAILED_STATE
--ZMARSHALLINGERROR - failed to marshall a request; possibly, out of memory
--
function async(zh, path, string_completion, data) end


---gets the acl associated with a node.
--
--@param zh the zookeeper handle obtained by a call to  init
--@param path the name of the node. Expressed as a file name with slashes
--separating ancestors of the node.
--@param acl_completion the routine to invoke when the request completes. The completion
--will be triggered with one of the following codes passed in as the rc argument:
--ZOK operation completed successfully
--ZNONODE the node does not exist.
--ZNOAUTH the client does not have permission.
--@param data the data that will be passed to the completion routine when 
--the function completes.
--@return ZOK on success or one of the following errcodes on failure:
--ZBADARGUMENTS - invalid input parameters
--ZINVALIDSTATE - zhandle state is either ZOO_SESSION_EXPIRED_STATE or ZOO_AUTH_FAILED_STATE
--ZMARSHALLINGERROR - failed to marshall a request; possibly, out of memory
--
function aget_acl(zh, path, acl_completion, data) end


---sets the acl associated with a node.
--
--@param zh the zookeeper handle obtained by a call to  init
--@param path the name of the node. Expressed as a file name with slashes 
--separating ancestors of the node.
--@param version the expected version of the path.
--@param acl the acl to be set on the path.
--@param void_completion the routine to invoke when the request completes. The completion
--will be triggered with one of the following codes passed in as the rc argument:
--ZOK operation completed successfully
--ZNONODE the node does not exist.
--ZNOAUTH the client does not have permission.
--ZINVALIDACL invalid ACL specified
--ZBADVERSION expected version does not match actual version.
--@param data the data that will be passed to the completion routine when 
--the function completes.
--@return ZOK on success or one of the following errcodes on failure:
--ZBADARGUMENTS - invalid input parameters
--ZINVALIDSTATE - zhandle state is either ZOO_SESSION_EXPIRED_STATE or ZOO_AUTH_FAILED_STATE
--ZMARSHALLINGERROR - failed to marshall a request; possibly, out of memory
--
function aset_acl(zh, path, version, acl, void_completion, data) end


---return an error string.
--
--@param c return code
--@return string corresponding to the return code
--
function zerror(c) end


---specify application credentials.
--
--The application calls this function to specify its credentials for purposes
--of authentication. The server will use the security provider specified by
--the scheme parameter to authenticate the client connection. If the
--authentication request has failed:
--- the server connection is dropped
--- the watcher is called with the ZOO_AUTH_FAILED_STATE value as the state
--parameter.
--@param zh the zookeeper handle obtained by a call to  init
--@param scheme the id of authentication scheme. Natively supported:
--"digest" password-based authentication
--@param cert application credentials. The actual value depends on the scheme.
--@param void_completion the routine to invoke when the request completes. One of
--the following result codes may be passed into the completion callback:
--ZOK operation completed successfully.
--ZAUTHFAILED authentication failed.
--@param data the data that will be passed to the completion routine when the
--function completes.
--@return ZOK on success or one of the following errcodes on failure:
--ZBADARGUMENTS - invalid input parameters.
--ZINVALIDSTATE - zhandle state is either ZOO_SESSION_EXPIRED_STATE or ZOO_AUTH_FAILED_STATE.
--ZMARSHALLINGERROR - failed to marshall a request; possibly, out of memory.
--ZSYSTEMERROR - a system error occured.
--
function add_auth(zh, scheme, cert, void_completion, data) end


---checks if the current zookeeper connection state can't be recovered.
--
--The application must close the zhandle and try to reconnect.
--
--@param zh the zookeeper handle ( init)
--@return ZINVALIDSTATE if connection is unrecoverable
--
function is_unrecoverable(zh) end


---sets the debugging level for the library.
--
function set_debug_level(loglevel) end


---sets the stream to be used by the library for logging
--
--The zookeeper library uses stderr as its default log stream. Application
--must make sure the stream is writable. Passing in NULL resets the stream
--to its default value (stderr).
--
function set_log_stream(stream) end


---enable/disable quorum endpoint order randomization.
--
--Note: typically this method should NOT be used outside of testing.
--
--If passed a non-zero value, will make the client connect to quorum peers
--in the order as specified in the init() call.
--A zero value causes init() to permute the peer endpoints
--which is good for more even client connection distribution among the
--quorum peers.
--
function deterministic_conn_order(yesorno) end


---create a node synchronously.
--
--This method will create a node in ZooKeeper. A node can only be created if
--it does not already exists. The Create Flags affect the creation of nodes.
--If ZOO_EPHEMERAL flag is set, the node will automatically get removed if the
--client session goes away. If the ZOO_SEQUENCE flag is set, a unique
--monotonically increasing sequence number is appended to the path name.
--
--@param zh the zookeeper handle obtained by a call to  init
--@param path The name of the node. Expressed as a file name with slashes
--separating ancestors of the node.
--@param acl The initial ACL of the node. The ACL must not be null or empty.
--@param flags this parameter can be set to 0 for normal create or an OR
--of the Create Flags
--@return 1).one of the following codes are returned, 2) path buffer of the node:
--ZOK operation completed successfully.
--ZNONODE the parent node does not exist.
--ZNODEEXISTS the node already exists.
--ZNOAUTH the client does not have permission.
--ZNOCHILDRENFOREPHEMERALS cannot create children of ephemeral nodes.
--ZBADARGUMENTS - invalid input parameters.
--ZINVALIDSTATE - zhandle state is either ZOO_SESSION_EXPIRED_STATE or ZOO_AUTH_FAILED_STATE.
--ZMARSHALLINGERROR - failed to marshall a request; possibly, out of memory.
--
function create(zh, path, value, acl, flags) end


---delete a node in zookeeper synchronously.
--
--@param zh the zookeeper handle obtained by a call to  init
--@param path the name of the node. Expressed as a file name with slashes
--separating ancestors of the node.
--@param version the expected version of the node. The function will fail if the
--   actual version of the node does not match the expected version.
-- If -1 is used the version check will not take place.
--@return one of the following values is returned.
--ZOK operation completed successfully.
--ZNONODE the node does not exist.
--ZNOAUTH the client does not have permission.
--ZBADVERSION expected version does not match actual version.
--ZNOTEMPTY children are present; node cannot be deleted.
--ZBADARGUMENTS - invalid input parameters
--ZINVALIDSTATE - zhandle state is either ZOO_SESSION_EXPIRED_STATE or ZOO_AUTH_FAILED_STATE.
--ZMARSHALLINGERROR - failed to marshall a request; possibly, out of memory.
--
function delete(zh, path, version) end


---checks the existence of a node in zookeeper synchronously.
--
--@param zh the zookeeper handle obtained by a call to  init
--@param path the name of the node. Expressed as a file name with slashes
--separating ancestors of the node.
--@param watch if nonzero, a watch will be set at the server to notify the
--client if the node changes. The watch will be set even if the node does not
--exist. This allows clients to watch for nodes to appear.
--@return 1): return code of the function call, 2): stat struct fo the ZNode.
--ZOK operation completed successfully.
--ZNONODE the node does not exist.
--ZNOAUTH the client does not have permission.
--ZBADARGUMENTS - invalid input parameters.
--ZINVALIDSTATE - zhandle state is either ZOO_SESSION_EXPIRED_STATE or ZOO_AUTH_FAILED_STATE.
--ZMARSHALLINGERROR - failed to marshall a request; possibly, out of memory.
--
function exists(zh, path, watch) end


---checks the existence of a node in zookeeper synchronously.
--
--This function is similar to  exists except it allows one specify
--a watcher object rather than a boolean watch flag.
--
--@param zh the zookeeper handle obtained by a call to  init
--@param path the name of the node. Expressed as a file name with slashes
--separating ancestors of the node.
--@param watcher_fn if non-null a watch will set on the specified znode on the server.
--The watch will be set even if the node does not exist. This allows clients
--to watch for nodes to appear.
--@param watcherctx user specific data, will be passed to the watcher callback.
--Unlike the global context set by  init, this watcher context
--is associated with the given instance of the watcher only.
--@return 1): return code of the function call, 2): stat struc of the ZNode.
--ZOK operation completed successfully.
--ZNONODE the node does not exist.
--ZNOAUTH the client does not have permission.
--ZBADARGUMENTS - invalid input parameters.
--ZINVALIDSTATE - zhandle state is either ZOO_SESSION_EXPIRED_STATE or ZOO_AUTH_FAILED_STATE.
--ZMARSHALLINGERROR - failed to marshall a request; possibly, out of memory.
--
function wexists(zh, path, watcher_fn, watcherctx) end


---gets the data associated with a node synchronously.
--
--@param zh the zookeeper handle obtained by a call to  init
--@param path the name of the node. Expressed as a file name with slashes
--separating ancestors of the node.
--@param watch if nonzero, a watch will be set at the server to notify
--the client if the node changes.
--It'll be set to the actual data length upon return. If the data is NULL, length is -1.
--@return 1): return value of the function call, 2): value buffer containing the node data, 3): stat struct of the ZNode.
--ZOK operation completed successfully.
--ZNONODE the node does not exist.
--ZNOAUTH the client does not have permission.
--ZBADARGUMENTS - invalid input parameters
--ZINVALIDSTATE - zhandle state is either in ZOO_SESSION_EXPIRED_STATE or ZOO_AUTH_FAILED_STATE.
--ZMARSHALLINGERROR - failed to marshall a request; possibly, out of memory.
--
function get(zh, path, watch) end


---gets the data associated with a node synchronously.
--
--This function is similar to  get except it allows one specify
--a watcher object rather than a boolean watch flag.
--
--@param zh the zookeeper handle obtained by a call to  init
--@param path the name of the node. Expressed as a file name with slashes
--separating ancestors of the node.
--@param watcher_fn if non-null, a watch will be set at the server to notify
--the client if the node changes.
--@param watcherctx user specific data, will be passed to the watcher callback.
--Unlike the global context set by  init, this watcher context
--is associated with the given instance of the watcher only.
--It'll be set to the actual data length upon return. If the data is NULL, length is -1.
--@return 1): return value of the function call, 2): buffer holding the node data, 3): stat struct of the ZNode.
--ZOK operation completed successfully.
--ZNONODE the node does not exist.
--ZNOAUTH the client does not have permission.
--ZBADARGUMENTS - invalid input parameters.
--ZINVALIDSTATE - zhandle state is either in ZOO_SESSION_EXPIRED_STATE or ZOO_AUTH_FAILED_STATE.
--ZMARSHALLINGERROR - failed to marshall a request; possibly, out of memory.
--
function wget(zh, path, watcher_fn, watcherctx) end


---sets the data associated with a node.
--
--See set2 function if you require access to the stat information
--associated with the znode.
--
--@param zh the zookeeper handle obtained by a call to  init
--@param path the name of the node. Expressed as a file name with slashes
--separating ancestors of the node.
--@param buffer the buffer holding data to be written to the node.
--@param version the expected version of the node. The function will fail if
--the actual version of the node does not match the expected version. If -1 is
--used the version check will not take place.
--@return the return code for the function call.
--ZOK operation completed successfully.
--ZNONODE the node does not exist.
--ZNOAUTH the client does not have permission.
--ZBADVERSION expected version does not match actual version.
--ZBADARGUMENTS - invalid input parameters.
--ZINVALIDSTATE - zhandle state is either ZOO_SESSION_EXPIRED_STATE or ZOO_AUTH_FAILED_STATE.
--ZMARSHALLINGERROR - failed to marshall a request; possibly, out of memory.
--
function set(zh, path, buffer, version) end


---sets the data associated with a node.
--
--This function is the same
--as set except that it also provides access to stat information
--associated with the znode.
--
--@param zh the zookeeper handle obtained by a call to  init
--@param path the name of the node. Expressed as a file name with slashes
--separating ancestors of the node.
--@param buffer the buffer holding data to be written to the node.
--@param buflen the number of bytes from buffer to write. To set NULL as data
--use buffer as NULL and buflen as -1.
--@param version the expected version of the node. The function will fail if
--the actual version of the node does not match the expected version. If -1 is
--used the version check will not take place.
--@return 1): the return code for the function call, 2): stat struct of the ZNode.
--ZOK operation completed successfully.
--ZNONODE the node does not exist.
--ZNOAUTH the client does not have permission.
--ZBADVERSION expected version does not match actual version.
--ZBADARGUMENTS - invalid input parameters.
--ZINVALIDSTATE - zhandle state is either ZOO_SESSION_EXPIRED_STATE or ZOO_AUTH_FAILED_STATE.
--ZMARSHALLINGERROR - failed to marshall a request; possibly, out of memory.
--
function set2(zh, path, buffer, version) end


--- lists the children of a node synchronously.
--
--@param zh the zookeeper handle obtained by a call to  init.
--@param path the name of the node. Expressed as a file name with slashes.
--separating ancestors of the node.
--@param watch if nonzero, a watch will be set at the server to notify.
--the client if the node changes.
--@param strings return value of children paths.
--@return the return code of the function.
--ZOK operation completed successfully.
--ZNONODE the node does not exist.
--ZNOAUTH the client does not have permission.
--ZBADARGUMENTS - invalid input parameters.
--ZINVALIDSTATE - zhandle state is either ZOO_SESSION_EXPIRED_STATE or ZOO_AUTH_FAILED_STATE.
--ZMARSHALLINGERROR - failed to marshall a request; possibly, out of memory.
--
function get_children(zh, path, watch, strings) end


---lists the children of a node synchronously.
--
--This function is similar to  get_children except it allows one specify
--a watcher object rather than a boolean watch flag.
--
--@param zh the zookeeper handle obtained by a call to  init
--@param path the name of the node. Expressed as a file name with slashes
--separating ancestors of the node.
--@param watcher_fn if non-null, a watch will be set at the server to notify
--the client if the node changes.
--@param watcherctx user specific data, will be passed to the watcher callback.
--Unlike the global context set by  init, this watcher context
--is associated with the given instance of the watcher only.
--@return 1): the return code of the function, 2): value of children paths.
--ZOK operation completed successfully.
--ZNONODE the node does not exist.
--ZNOAUTH the client does not have permission.
--ZBADARGUMENTS - invalid input parameters.
--ZINVALIDSTATE - zhandle state is either ZOO_SESSION_EXPIRED_STATE or ZOO_AUTH_FAILED_STATE.
--ZMARSHALLINGERROR - failed to marshall a request; possibly, out of memory.
--
function wget_children(zh, path, watcher_fn, watcherctx) end


---lists the children of a node and get its stat synchronously.
--
--This function is new in version 3.3.0
--
--@param zh the zookeeper handle obtained by a call to  init
--@param path the name of the node. Expressed as a file name with slashes
--separating ancestors of the node.
--@param watch if nonzero, a watch will be set at the server to notify
--the client if the node changes.
--@return 1): the return code of the function, 2): value of children paths, 3): value of node stat.
--ZOK operation completed successfully.
--ZNONODE the node does not exist.
--ZNOAUTH the client does not have permission.
--ZBADARGUMENTS - invalid input parameters.
--ZINVALIDSTATE - zhandle state is either ZOO_SESSION_EXPIRED_STATE or ZOO_AUTH_FAILED_STATE.
--ZMARSHALLINGERROR - failed to marshall a request; possibly, out of memory.
--
function get_children2(zh, path, watch) end


---lists the children of a node and get its stat synchronously.
--
--This function is similar to  get_children except it allows one specify
--a watcher object rather than a boolean watch flag.
--
--This function is new in version 3.3.0
--
--@param zh the zookeeper handle obtained by a call to  init
--@param path the name of the node. Expressed as a file name with slashes
--separating ancestors of the node.
--@param watcher_fn if non-null, a watch will be set at the server to notify
--the client if the node changes.
--@param watcherctx user specific data, will be passed to the watcher callback.
--Unlike the global context set by  init, this watcher context
--is associated with the given instance of the watcher only.
--@return 1): the return code of the function, 2): value of children paths, 3): value of node stat.
--ZOK operation completed successfully.
--ZNONODE the node does not exist.
--ZNOAUTH the client does not have permission.
--ZBADARGUMENTS - invalid input parameters.
--ZINVALIDSTATE - zhandle state is either ZOO_SESSION_EXPIRED_STATE or ZOO_AUTH_FAILED_STATE.
--ZMARSHALLINGERROR - failed to marshall a request; possibly, out of memory.
--
function wget_children2(zh, path, watcher_fn , watcherctx) end


---gets the acl associated with a node synchronously.
--
--@param zh the zookeeper handle obtained by a call to  init
--@param path the name of the node. Expressed as a file name with slashes
--separating ancestors of the node.
--@return 1): the return code for the function call, 2): value of ACLs on the path, 3): stat of the path specified.
--ZOK operation completed successfully.
--ZNONODE the node does not exist.
--ZNOAUTH the client does not have permission.
--ZBADARGUMENTS - invalid input parameters.
--ZINVALIDSTATE - zhandle state is either ZOO_SESSION_EXPIRED_STATE or ZOO_AUTH_FAILED_STATE.
--ZMARSHALLINGERROR - failed to marshall a request; possibly, out of memory.
--
function get_acl(zh, path) end


---sets the acl associated with a node synchronously.
--
--@param zh the zookeeper handle obtained by a call to  init
--@param path the name of the node. Expressed as a file name with slashes
--separating ancestors of the node.
--@param version the expected version of the path.
--@param acl the acl to be set on the path.
--@return the return code for the function call.
--ZOK operation completed successfully.
--ZNONODE the node does not exist.
--ZNOAUTH the client does not have permission.
--ZINVALIDACL invalid ACL specified.
--ZBADVERSION expected version does not match actual version.
--ZBADARGUMENTS - invalid input parameters.
--ZINVALIDSTATE - zhandle state is either ZOO_SESSION_EXPIRED_STATE or ZOO_AUTH_FAILED_STATE.
--ZMARSHALLINGERROR - failed to marshall a request; possibly, out of memory.
--
function set_acl(zh, path, version, acl) end
