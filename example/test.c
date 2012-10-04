//
//  main.c
//  jsonrpc
//
//

#include <stdio.h>
#include <jsonrpc.h>
#include "../plugins/jsonrpc_plugin_yajl.h"

jsonrpc_error_t subtract (int argc, const jsonrpc_param_t *argv, void (* print_result)(void *ctx, const char *fmt, ...), void *ctx)
{
	double r;

	printf("%s(argc:%d)\n", __FUNCTION__, argc);

	r = argv[0].json.u.number - argv[1].json.u.number;

	print_result(ctx, "%lf", r);
	return JSONRPC_ERROR_OK;
}

jsonrpc_error_t sum (int argc, const jsonrpc_param_t *argv, void (* print_result)(void *ctx, const char *fmt, ...), void *ctx)
{
	double r = 0.0;

	printf("%s(argc:%d)\n", __FUNCTION__, argc);

	while (argc--)
	{
		r += argv[argc].json.u.number;
	}

	print_result(ctx, "%lf", r);
	return JSONRPC_ERROR_OK;
}

jsonrpc_error_t multiply (int argc, const jsonrpc_param_t *argv, void (* print_result)(void *ctx, const char *fmt, ...), void *ctx)
{
	double r;

	printf("%s(argc:%d)\n", __FUNCTION__, argc);

	r = argv[0].json.u.number * argv[1].json.u.number;

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
	const char       *req, *res;

	server = jsonrpc_server_open(jsonrpc_plugin_yajl());

	error  = jsonrpc_server_register_method(server, JSONRPC_TRUE, subtract, "subtract", "minuend:i, subtrahend:i");
	error  = jsonrpc_server_register_method(server, JSONRPC_TRUE, sum, "sum", "iii");
	error  = jsonrpc_server_register_method(server, JSONRPC_FALSE, update, "update", "iiiii");
	error  = jsonrpc_server_register_method(server, JSONRPC_FALSE, foobar, "foobar", NULL);
	error  = jsonrpc_server_register_method(server, JSONRPC_TRUE, get_data, "get_data", NULL);
	error  = jsonrpc_server_register_method(server, JSONRPC_FALSE, test_param, "test.param", "sboai");

	printf("[rpc call with positional parameters]\n");
	{
		req = "{\"jsonrpc\": \"2.0\", \"method\": \"subtract\", \"params\": [42, 23], \"id\": 1}";
		res = jsonrpc_server_execute(server, req);
		printf("--> %s\n<-- %s\n\n", req, res);

		req = "{\"jsonrpc\": \"2.0\", \"method\": \"subtract\", \"params\": [23, 42], \"id\": 1}";
		res = jsonrpc_server_execute(server, req);
		printf("--> %s\n<-- %s\n\n", req, res);
	}

	printf("[rpc call with named parameters:]\n");
	{
		req = "{\"jsonrpc\": \"2.0\", \"method\": \"subtract\", \"params\": {\"subtrahend\": 23, \"minuend\": 42}, \"id\": 3}";
		res = jsonrpc_server_execute(server, req);
		printf("--> %s\n<-- %s\n\n", req, res);

		req = "{\"jsonrpc\": \"2.0\", \"method\": \"subtract\", \"params\": {\"minuend\": 42, \"subtrahend\": 23}, \"id\": 4}";
		res = jsonrpc_server_execute(server, req);
		printf("--> %s\n<-- %s\n\n", req, res);
	}


	jsonrpc_server_close(server);  
	return 0;
}

