/*
 * Copyright (c) 2012 Jonghyeok Lee <jhlee4bb@gmail.com>
 *
 * jsonrpC is free software; you can redistribute it and/or modify
 * it under the terms of the MIT license. See LICENSE for details.
 */

#include <stdio.h>
#include <math.h>
#include <jsonrpc.h>

#include <libwebsockets.h>

#include "../plugins/jsonrpc_plugin_yajl.h"

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

static jsonrpc_server_t *	jsonrpc_server;
static jsonrpc_server_t *	get_jsonrpc_server (void)
{
	if (jsonrpc_server == NULL)
	{
		jsonrpc_server_t *server;
		jsonrpc_error_t   error;
		
		server = jsonrpc_server_open(jsonrpc_plugin_yajl());
		error  = jsonrpc_server_register_method(server, JSONRPC_TRUE, subtract, "subtract", "minuend:i, subtrahend:i");
		error  = jsonrpc_server_register_method(server, JSONRPC_TRUE, sum, "sum", "iii");
		error  = jsonrpc_server_register_method(server, JSONRPC_FALSE, update, "update", "iiiii");
		error  = jsonrpc_server_register_method(server, JSONRPC_FALSE, foobar, "foobar", NULL);
		error  = jsonrpc_server_register_method(server, JSONRPC_TRUE, get_data, "get_data", NULL);
		error  = jsonrpc_server_register_method(server, JSONRPC_FALSE, test_param, "test.param", "sboai");
		jsonrpc_server = server;
	}
	return jsonrpc_server;
}




static void
dump_handshake_info(struct lws_tokens *lwst)
{
	int n;
	static const char *token_names[WSI_TOKEN_COUNT] = {
		/*[WSI_TOKEN_GET_URI]		=*/ "GET URI",
		/*[WSI_TOKEN_HOST]		=*/ "Host",
		/*[WSI_TOKEN_CONNECTION]	=*/ "Connection",
		/*[WSI_TOKEN_KEY1]		=*/ "key 1",
		/*[WSI_TOKEN_KEY2]		=*/ "key 2",
		/*[WSI_TOKEN_PROTOCOL]		=*/ "Protocol",
		/*[WSI_TOKEN_UPGRADE]		=*/ "Upgrade",
		/*[WSI_TOKEN_ORIGIN]		=*/ "Origin",
		/*[WSI_TOKEN_DRAFT]		=*/ "Draft",
		/*[WSI_TOKEN_CHALLENGE]		=*/ "Challenge",
		
		/* new for 04 */
		/*[WSI_TOKEN_KEY]		=*/ "Key",
		/*[WSI_TOKEN_VERSION]		=*/ "Version",
		/*[WSI_TOKEN_SWORIGIN]		=*/ "Sworigin",
		
		/* new for 05 */
		/*[WSI_TOKEN_EXTENSIONS]	=*/ "Extensions",
		
		/* client receives these */
		/*[WSI_TOKEN_ACCEPT]		=*/ "Accept",
		/*[WSI_TOKEN_NONCE]		=*/ "Nonce",
		/*[WSI_TOKEN_HTTP]		=*/ "Http",
		/*[WSI_TOKEN_MUXURL]	=*/ "MuxURL",
	};
	
	for (n = 0; n < WSI_TOKEN_COUNT; n++) {
		if (lwst[n].token == NULL)
			continue;
		
		printf("(%s) %s = %s\n", __FUNCTION__, token_names[n], lwst[n].token);
	}
}

#define	MAX_RESPONSE_Q	32
typedef struct jsonrpc_response
{
	struct jsonrpc_response *next;
	size_t		size;
	char		data[4];
} jsonrpc_response_t;

typedef struct jsonrpc_session {
	struct libwebsocket *wsi;
	jsonrpc_server_t	*jsonrpc_server;
	
	jsonrpc_response_t	*head;
	jsonrpc_response_t	*tail;
} jsonrpc_session_t;

static void	push_response (jsonrpc_session_t *session, const char *response)
{
	jsonrpc_response_t	*r;
	size_t	len;
	
	if (response == NULL)
		return;
	len = strlen(response);
	if (len == 0)
		return;
	
	r = (jsonrpc_response_t *)calloc(1, sizeof(jsonrpc_response_t) + len);
	if (r == NULL)
		return;
	
	r->size = len;
	memcpy(r->data, response, len);
	
	if (session->tail == NULL)
		session->head = session->tail = r;
	else
	{
		session->tail->next = r;
		session->tail = r;
	}
}

static jsonrpc_response_t *	pop_response (jsonrpc_session_t *session)
{
	jsonrpc_response_t *response;
	
	response = session->head;
	if (response == NULL)
		return NULL;
	
	session->head = response->next;
	if (session->head == NULL)
		session->tail = NULL;
	return response;
}

static int websocket_listener (struct libwebsocket_context *context,
		struct libwebsocket *wsi,
		enum libwebsocket_callback_reasons reason, void *user,
							   void *in, size_t len)
{
	jsonrpc_session_t  *session = (jsonrpc_session_t *)user;
	jsonrpc_response_t *response;
	int	n;
	const char *result;
	
	switch (reason)
	{
		case LWS_CALLBACK_ESTABLISHED:
			//fprintf(stderr, "%s(LWS_CALLBACK_ESTABLISHED)\n", __FUNCTION__);
			session->jsonrpc_server = get_jsonrpc_server();
			session->wsi = wsi;
			break;

		case LWS_CALLBACK_SERVER_WRITEABLE:
			//fprintf(stderr, "%s(LWS_CALLBACK_SERVER_WRITEABLE)\n", __FUNCTION__);
			response = pop_response(session);
			if (response)
			{
				n = libwebsocket_write(wsi, (unsigned char *)response->data, response->size, LWS_WRITE_TEXT);
				free(response);
				
				if (n < 0)
				{
					fprintf(stderr, "ERROR writing to socket\n");
					break;
				}
				libwebsocket_callback_on_writable(context, wsi);
			}
			break;
			
		case LWS_CALLBACK_BROADCAST:
			//fprintf(stderr, "%s(LWS_CALLBACK_BROADCAST)\n", __FUNCTION__);
			
			n = libwebsocket_write(wsi, in, len, LWS_WRITE_TEXT);
			if (n < 0)
				fprintf(stderr, "mirror write failed\n");
			break;

		case LWS_CALLBACK_RECEIVE:
			//fprintf(stderr, "%s(LWS_CALLBACK_RECEIVE)\n", __FUNCTION__);
			
			result = jsonrpc_server_execute(session->jsonrpc_server, in);
			if (result)
			{
				push_response(session, result);
				libwebsocket_callback_on_writable_all_protocol(libwebsockets_get_protocol(wsi));
			}
			break;
			
			/*
			 * this just demonstrates how to use the protocol filter. If you won't
			 * study and reject connections based on header content, you don't need
			 * to handle this callback
			 */

		case LWS_CALLBACK_FILTER_PROTOCOL_CONNECTION:
			dump_handshake_info((struct lws_tokens *)(long)user);
			/* you could return non-zero here and kill the connection */
			break;

		default:
			break;
	}
	return 0;

}

static int jsonrpc_websockset_server (void)
{
	struct libwebsocket_context *ws_ctx;
	static struct libwebsocket_protocols protocols[] =
	{
		{"jsonrpc-server-websocket", websocket_listener, sizeof(jsonrpc_session_t), },
		{NULL, NULL, 0		/* End of list */}
	};

	ws_ctx = libwebsocket_create_context(
				8212, NULL, protocols, libwebsocket_internal_extensions, NULL, NULL, -1, -1, 0
			);
	for (;;)
	{
		libwebsocket_service(ws_ctx, 50);
	}
	libwebsocket_context_destroy(ws_ctx);
	if (jsonrpc_server)
		jsonrpc_server_close(jsonrpc_server);
	
	return 0;
}

int main (int argc, const char * argv[])
{
	return jsonrpc_websockset_server();
}

