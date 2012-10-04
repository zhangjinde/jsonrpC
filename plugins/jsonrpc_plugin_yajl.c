//
//  jsonrpc_plugin_yajl.c
//  jsonrpc
//
//


#include "jsonrpc_plugin_yajl.h"
#include <jsonrpc_macro.h>

#include <yajl/yajl_tree.h>


static jsonrpc_handle_t	jsonrpc_yajl_parse  (const char *json)
{
#ifdef	JSONRPC_DEBUG
	char     errbuf[1024];
	yajl_val root = yajl_tree_parse(json, errbuf, 1024);
	if (!root)
	{
		fprintf(stderr, "%s() error: %s\n", __FUNCTION__, errbuf);
	}
#else
	yajl_val root = yajl_tree_parse(json, NULL, 0);
#endif
	return (jsonrpc_handle_t)root;
}

static void				jsonrpc_yajl_release(jsonrpc_handle_t json)
{
	yajl_tree_free((yajl_val)json);
}

static jsonrpc_handle_t	jsonrpc_yajl_get    (jsonrpc_handle_t json, const char *key)
{
	const char *path[2];

	path[0] = key;
	path[1] = NULL;
	return (jsonrpc_handle_t)yajl_tree_get((yajl_val)json, path, yajl_t_any);
}

static jsonrpc_handle_t	jsonrpc_yajl_get_at (jsonrpc_handle_t json, size_t index)
{
	yajl_val v = (yajl_val)json;

	if (v->type == yajl_t_object)
	{
		if (v->u.object.len > index)
			return (jsonrpc_handle_t)v->u.object.values[index];
	}
	else if (v->type == yajl_t_array)
	{
		if (v->u.array.len > index)
			return (jsonrpc_handle_t)v->u.array.values[index];
	}
	return NULL;
}

static const char *		jsonrpc_yajl_get_key_at(jsonrpc_handle_t json, size_t index)
{
	yajl_val v = (yajl_val)json;

	if (v->type == yajl_t_object)
	{
		if (v->u.object.len > index)
			return (jsonrpc_handle_t)v->u.object.keys[index];
	}
	return NULL;
}

static jsonrpc_bool_t		jsonrpc_yajl_valueof (jsonrpc_handle_t json, jsonrpc_json_t *value)
{
	yajl_val v = (yajl_val)json;

	switch (v->type)
	{
	case yajl_t_string:
		value->type = JSONRPC_TYPE_STRING;
		if (v->u.string == NULL)
			return JSONRPC_FALSE;
		value->u.string = v->u.string;
		break;

	case yajl_t_number:
		value->type = JSONRPC_TYPE_NUMBER;
		if (v->u.number.flags & YAJL_NUMBER_DOUBLE_VALID)
			value->u.number = v->u.number.d;
		else if (v->u.number.flags & YAJL_NUMBER_INT_VALID)
			value->u.number = (double)v->u.number.i;
		else
			return JSONRPC_FALSE;
		break;

	case yajl_t_object:
		value->type = JSONRPC_TYPE_OBJECT;
		value->u.object = (jsonrpc_handle_t)json;
		break;

	case yajl_t_array:
		value->type = JSONRPC_TYPE_ARRAY;
		value->u.array = (jsonrpc_handle_t)json;
		break;

	case yajl_t_true:
		value->type = JSONRPC_TYPE_BOOLEAN;
		value->u.boolean = JSONRPC_TRUE;
		break;

	case yajl_t_false:
		value->type = JSONRPC_TYPE_BOOLEAN;
		value->u.boolean = JSONRPC_FALSE;
		break;

	case yajl_t_null:
		value->type = JSONRPC_TYPE_NULL;
		break;

    default:
		return JSONRPC_FALSE;
	}
	return JSONRPC_TRUE;
}

static size_t				jsonrpc_yajl_length(jsonrpc_handle_t json)
{
	yajl_val v = (yajl_val)json;

	if (v->type == yajl_t_object)
	{
		return v->u.object.len;
	}
	else if (v->type == yajl_t_array)
	{
		return v->u.array.len;
	}
	return 0;
}


const jsonrpc_json_plugin_t	* jsonrpc_plugin_yajl (void)
{
	static const jsonrpc_json_plugin_t plugin_yajl = {
		jsonrpc_yajl_parse,
		jsonrpc_yajl_release,
		jsonrpc_yajl_get,
		jsonrpc_yajl_get_at,
		jsonrpc_yajl_get_key_at,
		jsonrpc_yajl_valueof,
		jsonrpc_yajl_length
	};
	return &plugin_yajl;
}


