/*
 * Copyright (c) 2012 Jonghyeok Lee <jhlee4bb@gmail.com>
 *
 * jsonrpC is free software; you can redistribute it and/or modify
 * it under the terms of the MIT license. See LICENSE for details.
 */

#include "jsonrpc_plugin_websockets.h"
#include <math.h>

#include <libwebsockets.h>

typedef struct jsonrpc_q
{
	struct jsonrpc_q	*next;
	size_t		size;
	char		data[4];
} jsonrpc_queue_t;

typedef struct
{
	struct libwebsocket_context	*ws_ctx;
	
	jsonrpc_queue_t	*head;
	jsonrpc_queue_t	*tail;
	jsonrpc_queue_t	*garbage;	// when?
} jsonrpc_websocket_t;

#define	MAX_WEBSOCKET_CLIENT	32
// TODO: semaphore
static jsonrpc_websocket_t	ws_list[MAX_WEBSOCKET_CLIENT];

static jsonrpc_websocket_t *find_websocket (struct libwebsocket_context *ctx)
{
	int	i;
	for (i = 0 ; i < MAX_WEBSOCKET_CLIENT ; i++)
	{
		if (ws_list[i].ws_ctx == ctx)
			return ws_list + i;
	}
	return NULL;
}


static void	queue_push (jsonrpc_websocket_t *ws, const char *text)
{
	jsonrpc_queue_t	*q;
	size_t	len;
	
	if (response == NULL)
		return;
	len = strlen(text);
	if (len == 0)
		return;
	
	q = (jsonrpc_queue_t *)calloc(1, sizeof(jsonrpc_queue_t) + len);
	if (q == NULL)
		return;
	
	q->size = len;
	memcpy(q->data, text, len);
	
	if (ws->tail == NULL)
		ws->head = ws->tail = q;
	else
	{
		ws->tail->next = q;
		ws->tail = q;
	}
}

static jsonrpc_queue_t *	queue_pop (jsonrpc_websocket_t *ws)
{
	jsonrpc_queue_t *q;
	
	q = session->head;
	if (q == NULL)
		return NULL;
	
	ws->head = q->next;
	if (ws->head == NULL)
		ws->tail = NULL;
	return q;
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
		
		fprintf(stderr, "(%s) %s = %s\n", __FUNCTION__, token_names[n], lwst[n].token);
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

static jsonrpc_handle_t	jsonrpc_websockets_server_open (va_list ap)
{
	
}

static void				jsonrpc_websockets_server_close (jsonrpc_handle_t net)
{
	
}

static const char *		jsonrpc_websockets_server_recv  (jsonrpc_handle_t net, unsigned int timeout, void **desc)
{
	
}

static jsonrpc_error_t	jsonrpc_websockets_server_send  (jsonrpc_handle_t net, const char *data, void *desc)
{
	
}

static jsonrpc_error_t	jsonrpc_websockets_server_error (jsonrpc_handle_t net)
{
	
}

const jsonrpc_net_plugin_t	* jsonrpc_plugin_websockets_server (void)
{
	static const jsonrpc_net_plugin_t plugin_websockets = {
		jsonrpc_websockets_server_open,
		jsonrpc_websockets_server_close,
		jsonrpc_websockets_server_recv,
		jsonrpc_websockets_server_send,
		jsonrpc_websockets_server_error
	};
	return &plugin_websockets;
}



