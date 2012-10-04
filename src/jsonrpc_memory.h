//
//  jsonrpc_memory.h
//  jsonrpc
//
//

#ifndef jsonrpc_jsonrpc_memory_h
#define jsonrpc_jsonrpc_memory_h

#include <stdio.h>
#include <stdarg.h>

void *  jsonrpc_malloc (size_t size);

void    jsonrpc_free (void *mem);

void *  jsonrpc_calloc (size_t count, size_t size);

void *  jsonrpc_realloc (void *mem, size_t size);

void *	jsonrpc_memdup (const void *mem, size_t size);

char *  jsonrpc_strdup (const char *str);

void    jsonrpc_vfree (void *mem, ...);

#endif
