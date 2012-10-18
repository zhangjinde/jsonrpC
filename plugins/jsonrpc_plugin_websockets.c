/*
 * Copyright (c) 2012 Jonghyeok Lee <jhlee4bb@gmail.com>
 *
 * jsonrpC is free software; you can redistribute it and/or modify
 * it under the terms of the MIT license. See LICENSE for details.
 */

#include "jsonrpc_plugin_websockets.h"
#include <math.h>

#include <libwebsockets.h>

typedef struct jsonrpc_ws_data
{
	struct jsonrpc_ws_data	*next;
	struct libwebsocket 	*wsi;
	size_t		size;
	char		data[4];
} jsonrpc_ws_data_t;

typedef struct
{
	jsonrpc_ws_data_t	*head;
	jsonrpc_ws_data_t	*tail;
} jsonrpc_queue_t;

typedef struct
{
	struct libwebsocket_context *ws_ctx;

	jsonrpc_queue_t			rx;
	jsonrpc_queue_t 		tx;
	jsonrpc_ws_data_t	*garbage;
} jsonrpc_websocket_t;

#define	MAX_WEBSOCKET_TEMP	64
// TODO: semaphore.. ??
static jsonrpc_websocket_t *ws_temp[MAX_WEBSOCKET_TEMP];

static void	push_websocket_to_temp (jsonrpc_websocket_t *ws)
{
	int n = MAX_WEBSOCKET_TEMP;
	while (n--)
	{
		if (ws_temp[n] == NULL)
		{
			ws_temp[n] = ws;
			break;
		}
	}
}

static jsonrpc_websocket_t *	pop_websocket_from_temp (struct libwebsocket_context *ctx)
{
	int n = MAX_WEBSOCKET_TEMP;
	while (n--)
	{
		if (ws_temp[n] && ws_temp[n]->ws_ctx == ctx)
		{
			jsonrpc_websocket_t *ws = ws_temp[n];
			ws_temp[n] = NULL;
			return ws;
		}
	}
	return NULL;
}


static jsonrpc_ws_data_t *	queue_push (jsonrpc_queue_t *queue, const char *text)
{
	jsonrpc_ws_data_t	*q;
	size_t	len;
	
	if (text == NULL)
		return NULL;
	len = strlen(text);
	if (len == 0)
		return NULL;
	
	q = (jsonrpc_ws_data_t *)calloc(1, sizeof(jsonrpc_ws_data_t) + len);
	if (q == NULL)
		return NULL;
	
	q->size = len;
	memcpy(q->data, text, len);
	
	if (queue->tail == NULL)
		queue->head = queue->tail = q;
	else
	{
		queue->tail->next = q;
		queue->tail = q;
	}
	return q;
}

static jsonrpc_ws_data_t *	queue_pop (jsonrpc_queue_t *queue)
{
	jsonrpc_ws_data_t *q;
	
	q = queue->head;
	if (q == NULL)
		return NULL;
	
	queue->head = q->next;
	if (queue->head == NULL)
		queue->tail = NULL;
	q->next = NULL;
	return q;
}

static void	queue_remove_all (jsonrpc_queue_t *queue)
{
	jsonrpc_ws_data_t *freed, *item;

	item = queue->head;
	while (item)
	{
		freed = item;
		item = item->next;
		free(freed);
	}
}

static void	queue_put_into_trash (jsonrpc_websocket_t *ws, jsonrpc_ws_data_t *item)
{
	item->next = ws->garbage;
	ws->garbage = item;
}

static void	queue_gc (jsonrpc_websocket_t *ws)
{
	jsonrpc_ws_data_t *freed, *garbage;

	garbage = ws->garbage;
	while (garbage)
	{
		freed = garbage;
		garbage = garbage->next;
		free(freed);
	}
	ws->garbage = NULL;
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


static int websocket_listener (struct libwebsocket_context *context,
							   struct libwebsocket *wsi,
							   enum libwebsocket_callback_reasons reason, void *user,
							   void *in, size_t len)
{
	jsonrpc_websocket_t *session;
	jsonrpc_ws_data_t   *data;
	int	n;
	
	switch (reason)
	{
		case LWS_CALLBACK_ESTABLISHED:
			//fprintf(stderr, "%s(LWS_CALLBACK_ESTABLISHED)\n", __FUNCTION__);
			session = pop_websocket_from_temp(context);
			*(jsonrpc_websocket_t **)user = session;			
			break;
			
		case LWS_CALLBACK_SERVER_WRITEABLE:
			//fprintf(stderr, "%s(LWS_CALLBACK_SERVER_WRITEABLE)\n", __FUNCTION__);
			session = *(jsonrpc_websocket_t **)user;
			data = queue_pop(&session->tx);
			if (data)
			{
				n = libwebsocket_write(wsi, (unsigned char *)data->data, data->size, LWS_WRITE_TEXT);
				free(data);
				
				if (n < 0)
				{
					//fprintf(stderr, "ERROR writing to socket\n");
					break;
				}
				libwebsocket_callback_on_writable(context, wsi);
			}
			break;
			
		case LWS_CALLBACK_BROADCAST:
			//fprintf(stderr, "%s(LWS_CALLBACK_BROADCAST)\n", __FUNCTION__);
			session = *(jsonrpc_websocket_t **)user;
			
			n = libwebsocket_write(wsi, in, len, LWS_WRITE_TEXT);
			if (n < 0)
				//fprintf(stderr, "mirror write failed\n");
			break;
			
		case LWS_CALLBACK_RECEIVE:
			//fprintf(stderr, "%s(LWS_CALLBACK_RECEIVE)\n", __FUNCTION__);
			session = *(jsonrpc_websocket_t **)user;
			data    = queue_push(&session->rx, in);
			if (data)
				data->wsi = wsi;
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
	struct libwebsocket_context *ws_ctx;
	jsonrpc_websocket_t	*ws_server;
	int port;

	static struct libwebsocket_protocols protocols[] =
	{
		{"jsonrpc-server-websocket", websocket_listener, sizeof(jsonrpc_websocket_t **), },
		{NULL, NULL, 0		/* End of list */}
	};

	port   = va_arg(ap, int);
	ws_server = (jsonrpc_websocket_t *)calloc(1, sizeof(jsonrpc_websocket_t));
	if (!ws_server)
		return (jsonrpc_handle_t)NULL;

	ws_ctx = libwebsocket_create_context(port, NULL, protocols, libwebsocket_internal_extensions, NULL, NULL, -1, -1, 0);
	if (!ws_ctx)
	{
		free(ws_server);
		return (jsonrpc_handle_t)NULL;
	}
	ws_server->ws_ctx = ws_ctx;
	push_websocket_to_temp(ws_server);

	return ws_server;
}

static void				jsonrpc_websockets_server_close (jsonrpc_handle_t net)
{
	jsonrpc_websocket_t *ws;

	ws = (jsonrpc_websocket_t *)net;
	if (ws)
	{
		libwebsocket_context_destroy(ws->ws_ctx);
		queue_gc(ws);
		queue_remove_all(&ws->rx);
		queue_remove_all(&ws->tx);
		free(ws);
	}
}

static const char *		jsonrpc_websockets_server_recv  (jsonrpc_handle_t net, unsigned int timeout, void **desc)
{
	jsonrpc_websocket_t		*ws;
	jsonrpc_ws_data_t		*recv;
	int 					n = 2;

	ws = (jsonrpc_websocket_t *)net;
	queue_gc(ws);

	while (n--)
	{
		recv = queue_pop(&ws->rx);
		if (recv)
		{
			queue_put_into_trash(ws, recv);
			if (desc)
				*desc = recv->wsi;
			return recv->data;
		}
		if (n == 1)
			libwebsocket_service(ws->ws_ctx, timeout);
	}
	return NULL;
}

static jsonrpc_error_t	jsonrpc_websockets_server_send  (jsonrpc_handle_t net, const char *data, void *desc)
{
	jsonrpc_websocket_t *ws;

	ws = (jsonrpc_websocket_t *)net;

	queue_push(&ws->tx, data);

	libwebsocket_callback_on_writable_all_protocol(
		libwebsockets_get_protocol(desc)
	);
	return JSONRPC_ERROR_OK;
}

static jsonrpc_error_t	jsonrpc_websockets_server_error (jsonrpc_handle_t net)
{
	jsonrpc_websocket_t *ws;

	ws = (jsonrpc_websocket_t *)net;

	// TODO:

	return JSONRPC_ERROR_OK;
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



