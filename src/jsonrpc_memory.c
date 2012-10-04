/*
 * Copyright (c) 2012 Jonghyeok Lee <jhlee4bb@gmail.com>
 *
 * jsonrpC is free software; you can redistribute it and/or modify
 * it under the terms of the MIT license. See LICENSE for details.
 */


#include <stdio.h>
#include <string.h>
#include "jsonrpc.h"
#include "jsonrpc_memory.h"

static void *  jsonrpc_default_malloc (size_t size, void *userdata)
{
    (void)userdata;
    return malloc(size);
}

static void *  jsonrpc_default_realloc (void *mem, size_t size, void *userdata)
{
    (void)userdata;
    return realloc(mem, size);
}


static void    jsonrpc_default_free (void *mem, void *userdata)
{
    (void)userdata;
    free(mem);
}

static struct jsonrpc_memory
{
    void *  (* _malloc) (size_t, void *);
    void *  (* _realloc) (void *, size_t, void *);
    void    (* _free) (void *, void *);
    void *  userdata;
} s_memory = {
    jsonrpc_default_malloc,
    jsonrpc_default_realloc,
    jsonrpc_default_free,
    NULL
};


void *  jsonrpc_malloc (size_t size)
{
    return s_memory._malloc(size, s_memory.userdata);
}

void    jsonrpc_free (void *mem)
{
    s_memory._free(mem, s_memory.userdata);
}

void *  jsonrpc_calloc (size_t count, size_t size)
{
    void *mem = jsonrpc_malloc(count * size);
    if (mem)
        memset(mem, 0, count * size);
    return mem;
}

void *  jsonrpc_realloc (void *mem, size_t size)
{
    return s_memory._realloc(mem, size, s_memory.userdata);
}

void *	jsonrpc_memdup (const void *mem, size_t size)
{
	void *dup = jsonrpc_malloc(size);
	if (dup)
	{
		memcpy(dup, mem, size);
	}
	return dup;
}

char *  jsonrpc_strdup (const char *str)
{
    size_t  n = strlen(str);
    char    *dup;
    
    dup = (char *)jsonrpc_malloc(n + 1);
    if (dup)
    {
        memcpy(dup, str, n);
        dup[n] = '\0';
    }
    return dup;
}


void    jsonrpc_vfree (void *mem, ...)
{
    va_list ap;
    
    va_start(ap, mem);
    while (mem)
    {
        jsonrpc_free(mem);
        mem = va_arg(ap, void *);
    }
    va_end(ap);
}

void
jsonrpc_set_alloc_funcs (
                         void * (* _malloc) (size_t n, void *userdata)
                         , void * (* _realloc) (void *mem, size_t n, void *userdata)
                         , void (* _free)(void *mem, void *userdata)
                         , void *userdata
                         )
{
    s_memory._malloc = _malloc;
    s_memory._realloc = _realloc;
    s_memory._free = _free;
    s_memory.userdata = userdata;
}

