/*
 * Copyright (c) 2012 Jonghyeok Lee <jhlee4bb@gmail.com>
 *
 * jsonrpC is free software; you can redistribute it and/or modify
 * it under the terms of the MIT license. See LICENSE for details.
 */

#include <stdio.h>
#include <math.h>
#include <jsonrpc.h>


#include "../plugins/jsonrpc_plugin_yajl.h"
#include "../plugins/jsonrpc_plugin_websockets.h"


jsonrpc_error_t subtract (int argc, const jsonrpc_param_t *argv, void (* print_result)(void *ctx, const char *fmt, ...), void *ctx)
{
	double r, f;

	printf("%s(argc:%d)\n", __FUNCTION__, argc);

	r = argv[0].json.u.number - argv[1].json.u.number;
	f = fmod(r, 1.0);
	if (f == 0.0)
		print_result(ctx, "%.0lf", r);
	else
		print_result(ctx, "%lf", r);
	return JSONRPC_ERROR_OK;
}

jsonrpc_error_t sum (int argc, const jsonrpc_param_t *argv, void (* print_result)(void *ctx, const char *fmt, ...), void *ctx)
{
	double r = 0.0, f;

	printf("%s(argc:%d)\n", __FUNCTION__, argc);

	while (argc--)
	{
		r += argv[argc].json.u.number;
	}

	f = fmod(r, 1.0);
	if (f == 0.0)
		print_result(ctx, "%.0lf", r);
	else
		print_result(ctx, "%lf", r);
	return JSONRPC_ERROR_OK;
}

jsonrpc_error_t multiply (int argc, const jsonrpc_param_t *argv, void (* print_result)(void *ctx, const char *fmt, ...), void *ctx)
{
	double r, f;

	printf("%s(argc:%d)\n", __FUNCTION__, argc);

	r = argv[0].json.u.number * argv[1].json.u.number;
	f = fmod(r, 1.0);
	if (f == 0.0)
		print_result(ctx, "%.0lf", r);
	else
		print_result(ctx, "%lf", r);
	return JSONRPC_ERROR_OK;
}

jsonrpc_error_t get_data (int argc, const jsonrpc_param_t *argv, void (* print_result)(void *ctx, const char *fmt, ...), void *ctx)
{
	printf("%s(argc:%d)\n", __FUNCTION__, argc);

	print_result(ctx, "{\"data\":\"abcde\"}");
	return JSONRPC_ERROR_OK;
}



jsonrpc_error_t update (int argc, const jsonrpc_param_t *argv, void (* print_result)(void *ctx, const char *fmt, ...), void *ctx)
{
	printf("%s(argc:%d)\n", __FUNCTION__, argc);

	return JSONRPC_ERROR_OK;
}

jsonrpc_error_t foobar (int argc, const jsonrpc_param_t *argv, void (* print_result)(void *ctx, const char *fmt, ...), void *ctx)
{
	printf("%s(argc:%d)\n", __FUNCTION__, argc);

	return JSONRPC_ERROR_OK;
}


jsonrpc_error_t test_param (int argc, const jsonrpc_param_t *argv, void (* print_result)(void *ctx, const char *fmt, ...), void *ctx)
{
	printf("%s(argc:%d)\n", __FUNCTION__, argc);

	printf("\t%c:%s\n", (char)argv->json.type, argv->json.u.string);
	argv++;
	printf("\t%c:%d\n", (char)argv->json.type, argv->json.u.boolean);
	argv++;
	printf("\t%c:0x%X\n", (char)argv->json.type, argv->json.u.object);
	argv++;
	printf("\t%c:0x%X\n", (char)argv->json.type, argv->json.u.array);
	argv++;
	printf("\t%c:%lf\n", (char)argv->json.type, argv->json.u.number);
	argv++;

	return JSONRPC_ERROR_OK;
}

int main (int argc, const char * argv[])
{
	jsonrpc_server_t *server;
	jsonrpc_error_t   error;
	
	server = jsonrpc_server_open(
				jsonrpc_plugin_yajl(), 
				jsonrpc_plugin_websockets_server(), 
				8212
			);
	error  = jsonrpc_server_register_method(server, JSONRPC_TRUE, subtract, "subtract", "minuend:i, subtrahend:i");
	error  = jsonrpc_server_register_method(server, JSONRPC_TRUE, sum, "sum", "iii");
	error  = jsonrpc_server_register_method(server, JSONRPC_FALSE, update, "update", "iiiii");
	error  = jsonrpc_server_register_method(server, JSONRPC_FALSE, foobar, "foobar", NULL);
	error  = jsonrpc_server_register_method(server, JSONRPC_TRUE, get_data, "get_data", NULL);
	error  = jsonrpc_server_register_method(server, JSONRPC_FALSE, test_param, "test.param", "sboai");

	for (;;)
	{
		jsonrpc_server_run(server, 50);
	}
	jsonrpc_server_close(server);
	return 0;
	// return jsonrpc_websockset_server();
}

