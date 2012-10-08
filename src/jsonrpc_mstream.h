/*
 * Copyright (c) 2012 Jonghyeok Lee <jhlee4bb@gmail.com>
 *
 * jsonrpC is free software; you can redistribute it and/or modify
 * it under the terms of the MIT license. See LICENSE for details.
 */


#ifndef jsonrpc_jsonrpc_mstream_h
#define jsonrpc_jsonrpc_mstream_h

#include <stdio.h>
#include <stdarg.h>

#ifdef  __cplusplus
extern "C" {
#endif

typedef struct jsonrpc_mstream	jsonrpc_mstream_t;

/**
 * Open the memory stream.
 *
 * @return instance of 'jsonrpc_mstream_t'
 */
jsonrpc_mstream_t *
jsonrpc_mstream_open (void);

/**
 * Close the memory stream.
 *
 * @param mstream	instance of 'jsonrpc_mstream_t'
 */
void
jsonrpc_mstream_close (jsonrpc_mstream_t *mstream);

/**
 * Formatted input to memory stream
 *
 * @param mstream	instance of 'jsonrpc_mstream_t'
 * @param fmt	format (same as printf's)
 * @param ap	variable-length argument
 * @return The number of characters that would have been writtend. (if error, return -1)
 */
int
jsonrpc_mstream_vprint (jsonrpc_mstream_t *mstream, const char *fmt, va_list ap);

/**
 * Formatted input to memory stream
 *
 * @param mstream	instance of 'jsonrpc_mstream_t'
 * @param fmt	format (same as printf's)
 * @param ...	variable-length argument
 * @return The number of characters that would have been written. (if error, return -1)
 */
int
jsonrpc_mstream_print (jsonrpc_mstream_t *mstream, const char *fmt, ...);

/**
 * The number of written characters
 *
 * @param mstream	instance of 'jsonrpc_mstream_t'
 * @return The number of written characters
 */
size_t
jsonrpc_mstream_length (jsonrpc_mstream_t *mstream);

/**
 * Set the memory position indicator to the beginning of the stream.
 *
 * @param mstream	instance of 'jsonrpc_mstream_t'
 */
void
jsonrpc_mstream_rewind (jsonrpc_mstream_t *mstream);

/**
 * Get memory stream buffer
 *
 * @param mstream	instance of 'jsonrpc_mstream_t'
 * @return	'\0' terminated string
 */
const char *
jsonrpc_mstream_getbuf (jsonrpc_mstream_t *mstream);

#ifdef  __cplusplus
}
#endif

#endif
