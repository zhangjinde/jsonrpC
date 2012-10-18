jsonrpC
=======

[jsonrpC](http://jhlee4bb.github.com/jsonrpC) is an implementation of [JSON RPC 2.0](http://www.jsonrpc.org/specification) protocol in C.
* Simple API
* Portable
 * ANSI C.
 * JSON library can be easily ported. (e.g. [yajl](http://lloyd.github.com/yajl), [jansson](http://www.digip.org/jansson/))
 * Transport independent. Use this library with UDS, WebSocket, ...
* Tiny

jsonrpC is licensed under the [MIT License](http://www.opensource.org/licenses/mit-license.php)

##Compilation
```
$mkdir build
$cd build
$cmake ..
$make
build output left in jsonrpc-x.y
```

##Example
####Server
test_websocket.c
```C
jsonrpc_error_t subtract (int argc, const jsonrpc_param_t *argv, void (* print_result)(void *ctx, const char *fmt, ...), void *ctx)
{
	print_result(ctx, "%lf", 
		argv[0].json.u.number - argv[1].json.u.number
	);
	return JSONRPC_ERROR_OK;
}

int main (int argc, const char * argv[])
{
	jsonrpc_server_t *server;
	jsonrpc_error_t   error;
	
	server = jsonrpc_server_open(
				/* json_plugin: yajl       */jsonrpc_plugin_yajl(), 
				/*  net_plugin: websockets */jsonrpc_plugin_websockets_server(), 
				/* port */8212
			);
	error  = jsonrpc_server_register_method(server, JSONRPC_TRUE, subtract, "subtract", "minuend:i, subtrahend:i");
	// add more method here..

	for (;;)
	{
		error = jsonrpc_server_run(server, 50);
		if (error != JSONRPC_ERROR_OK && error != JSONRPC_ERROR_SERVER_TIMEOUT)
			break;
	}
	jsonrpc_server_close(server);
	return 0;
}
```

####Client
test_websocket.html
```javascript
var ws;

function init()
{
	ws = new WebSocket("ws://localhost:8212", "jsonrpc-server-websocket");
	ws.onopen = function(e) {
		writeLog('WebSocket: CONNECTED');
	};
	ws.onclose = function(e) {
		writeLog('WebSocket: DISCONNECTED');
	};
	ws.onmessage = function(e) {
		writeLog('<-- ' + e.data);
	};
	ws.onerror = function(e) {
		writeLog('WebSocket: ERROR(' + e.data + ')');
	};
}
function request()
{
	var json=document.getElementById("pre_code").value;
	
	writeLog('--> ' + json)
	ws.send(json);
}
function writeLog(text)
{
	document.getElementById("pre_output").value += text + '\n';
}
```
####Screenshot
open test_websocket.html (with WebSocket supported browser)
![screenshot](http://farm9.staticflickr.com/8454/8062570242_1aea4d2602.jpg)

##ToDo
* jsonrpC client library
* jansson json plugin

