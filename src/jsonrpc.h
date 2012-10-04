//
//  jsonrpc.h
//  jsonrpc
//
//

#ifndef _jsonrpc_h
#define _jsonrpc_h


#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#ifdef  __cplusplus
extern "C" {
#endif

// <jsonrpc_config.h>
#define	JSONRPC_VERSION				"2.0"

// </jsonrpc_config.h>

#define	JSONRPC_NAME_LEN		128

/**
 * JSON-RPC error codes
 * @see json-rpc.org
 */
typedef enum
{
	  JSONRPC_ERROR_OK					= 0
	, JSONRPC_ERROR_PARSE_ERROR		    = -32700
	, JSONRPC_ERROR_INVALID_REQUEST		= -32600
	, JSONRPC_ERROR_METHOD_NOT_FOUND	= -32601
	, JSONRPC_ERROR_INVALID_PARAMS		= -32602
	, JSONRPC_ERROR_INTERNAL			= -32603
	, JSONRPC_ERROR_RESERVED_FOR_SERVER_END		= -32099
		, JSONRPC_ERROR_SERVER_OUT_OF_MEMORY
		, JSONRPC_ERROR_SERVER_INTERNAL
	, JSONRPC_ERROR_RESERVED_FOR_SERVER_BEGIN	= -32000
} jsonrpc_error_t;


/**
 * JSON-RPC data types
 */
typedef enum {
	JSONRPC_TYPE_NUMBER    = 'i',
	JSONRPC_TYPE_BOOLEAN   = 'b',
	JSONRPC_TYPE_STRING    = 's',
	JSONRPC_TYPE_ARRAY	   = 'a',
	JSONRPC_TYPE_OBJECT    = 'o',
	JSONRPC_TYPE_NULL	   = 'n',
	JSONRPC_TYPE_UNDEFINED = 'u'
} jsonrpc_type_t;
#define JSONRPC_TYPES		"ibsaonu"

/**
 * JSON-RPC boolean type
 */
typedef enum {
	JSONRPC_FALSE,
	JSONRPC_TRUE
} jsonrpc_bool_t;

typedef void *	jsonrpc_handle_t;	///< handle type (general purpose)

/**
 * JSON-RPC json type
 *
 */
typedef struct
{
	jsonrpc_type_t		type;		///< type of the value
	union
	{
		double				number;		///< number value
		jsonrpc_bool_t		boolean;	///< boolean value
		char				*string;	///< string value
		jsonrpc_handle_t	array;		///< array json handle
		jsonrpc_handle_t	object;		///< object json handle
	} u;
} jsonrpc_json_t;

/**
 * JSON-RPC parameter
 *
 */
typedef struct
{
	/**
	 * name of param
	 */
	char				name[JSONRPC_NAME_LEN];
	size_t				index;
	jsonrpc_json_t		json;	///< json value
} jsonrpc_param_t;

/**
 * JSON-RPC json api plug-in
 * -
 */
typedef struct
{
	jsonrpc_handle_t	(* parse  ) (const char *json);
	void				(* release) (jsonrpc_handle_t json);
	jsonrpc_handle_t	(* get    ) (jsonrpc_handle_t json, const char *path);
	jsonrpc_handle_t	(* get_at ) (jsonrpc_handle_t json, size_t index);
	const char *		(* get_key_at) (jsonrpc_handle_t json, size_t index);
	jsonrpc_bool_t		(* valueof) (jsonrpc_handle_t json, jsonrpc_json_t *value);
	size_t				(* length) (jsonrpc_handle_t json);
} jsonrpc_json_plugin_t;

/**
 * JSON-RPC method
 * -
 * @param	argc	argument count
 * @param	argv	arguments
 * @param	print_result	write result
 * @param	ctx		print context
 */
typedef jsonrpc_error_t (* jsonrpc_method_t) (
								int argc
								, const jsonrpc_param_t *argv
								, void (* print_result)(void *ctx, const char *fmt, ...)
								, void *ctx
							);


void
jsonrpc_set_alloc_funcs (
				  void * (* _malloc) (size_t n, void *userdata)
				, void * (* _realloc) (void *mem, size_t n, void *userdata)
				, void (* _free)(void *mem, void *userdata)
				, void *userdata
			);


/**
 * JSON-RPC server
 *
 */
typedef struct jsonrpc_server	jsonrpc_server_t;

jsonrpc_server_t *
jsonrpc_server_open (
				const jsonrpc_json_plugin_t *ijson
			);

void
jsonrpc_server_close (jsonrpc_server_t *self);

jsonrpc_error_t
jsonrpc_server_register_method (
				jsonrpc_server_t *self
				, jsonrpc_bool_t has_return
				, jsonrpc_method_t method
				, const char *method_name
				, const char *param_signature
			);

const char *
jsonrpc_server_execute (jsonrpc_server_t *self, const char *request);


#ifdef  __cplusplus
}
#endif

#endif  //_jsonrpc_h
