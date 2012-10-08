/*
 * Copyright (c) 2012 Jonghyeok Lee <jhlee4bb@gmail.com>
 *
 * jsonrpC is free software; you can redistribute it and/or modify
 * it under the terms of the MIT license. See LICENSE for details.
 */


#ifndef jsonrpc_jsonrpc_memory_h
#define jsonrpc_jsonrpc_memory_h

#include <stdio.h>
#include <stdarg.h>

#ifdef  __cplusplus
extern "C" {
#endif
	
/**
 * same as 'malloc'
 */
void *  jsonrpc_malloc (size_t size);

/**
 * same as 'free'
 */
void    jsonrpc_free (void *mem);

/**
 * same as 'calloc'
 */
void *  jsonrpc_calloc (size_t count, size_t size);

/**
 * same as 'realloc'
 */
void *  jsonrpc_realloc (void *mem, size_t size);

/**
 * The function returns a pointer to a new memory which is a duplicated of the memory 'mem' with 'size'.
 */
void *	jsonrpc_memdup (const void *mem, size_t size);

/**
 * same as 'strdup'
 */
char *  jsonrpc_strdup (const char *str);

/**
 * example) jsonrpc_vfree(mem1, mem2, NULL);
 */
void    jsonrpc_vfree (void *mem, ...);

#ifdef  __cplusplus
}
#endif
		
#endif
