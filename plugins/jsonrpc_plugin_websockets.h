/*
 * Copyright (c) 2012 Jonghyeok Lee <jhlee4bb@gmail.com>
 *
 * jsonrpC is free software; you can redistribute it and/or modify
 * it under the terms of the MIT license. See LICENSE for details.
 */

#ifndef jsonrpc_jsonrpc_plugin_websockets_h
#define jsonrpc_jsonrpc_plugin_websockets_h

#include <stdio.h>
#include <stdarg.h>

#include <jsonrpc.h>

#ifdef  __cplusplus
extern "C" {
#endif

const jsonrpc_net_plugin_t	* jsonrpc_plugin_websockets_server (void);

#ifdef  __cplusplus
}
#endif
#endif

