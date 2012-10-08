/*
 * Copyright (c) 2012 Jonghyeok Lee <jhlee4bb@gmail.com>
 *
 * jsonrpC is free software; you can redistribute it and/or modify
 * it under the terms of the MIT license. See LICENSE for details.
 */


#include <stdio.h>
#include "jsonrpc_mstream.h"
#include "jsonrpc_macro.h"
#include "jsonrpc_memory.h"



struct jsonrpc_mstream
{
	char	*stream;
	size_t	alloc;
	size_t	length;
};

JSONRPC_PRIVATE size_t
mstream_grow (jsonrpc_mstream_t *mstream)
{
	void *grown;

	grown = jsonrpc_realloc(mstream->stream, mstream->alloc * 2);
	JSONRPC_THROW(grown == NULL, return 0);

	mstream->stream = (char *)grown;
	mstream->alloc  *= 2;

	return mstream->alloc - mstream->length - 1;	// return avaliable length
}

jsonrpc_mstream_t *
jsonrpc_mstream_open (void)
{
	jsonrpc_mstream_t	*mstream;

	mstream = (jsonrpc_mstream_t *)jsonrpc_calloc(1, sizeof(jsonrpc_mstream_t));
	if (mstream)
	{
		mstream->alloc  = 128;	// default
		mstream->stream = (char *)jsonrpc_malloc(sizeof(char) * mstream->alloc);
		JSONRPC_THROW(mstream->stream == NULL, {
			jsonrpc_free(mstream);
			return NULL;
		});
	}
	return mstream;
}

void
jsonrpc_mstream_close (jsonrpc_mstream_t *mstream)
{
	jsonrpc_vfree(mstream->stream, mstream, NULL);
}

int
jsonrpc_mstream_vprint (jsonrpc_mstream_t *mstream, const char *fmt, va_list ap)
{
	char	*stream;
	size_t	length;
	int		written;
	va_list	va;
	int		retry = 10;

	if (mstream->alloc - mstream->length <= 1)
	{
		JSONRPC_THROW(mstream_grow(mstream) == 0, return -1);
	}

	while (retry--)
	{
		va_copy(va, ap);
		stream  = mstream->stream + mstream->length;
		length  = mstream->alloc - mstream->length - 1/*for NULL*/;
		written = vsnprintf(stream, length, fmt, va);
		if (0 <= written && written < (int)length)
		{
			mstream->length += (size_t)written;
			return written;
		}
		JSONRPC_THROW(mstream_grow(mstream) == 0, return -1);
	}
	return -1;
}

int
jsonrpc_mstream_print (jsonrpc_mstream_t *mstream, const char *fmt, ...)
{
	int	ret;
	va_list	ap;

	va_start(ap, fmt);
	ret = jsonrpc_mstream_vprint(mstream, fmt, ap);
	va_end(ap);

	return ret;
}

size_t
jsonrpc_mstream_length (jsonrpc_mstream_t *mstream)
{
	return mstream->length;
}

void
jsonrpc_mstream_rewind (jsonrpc_mstream_t *mstream)
{
	mstream->length = 0;
}

const char *
jsonrpc_mstream_getbuf (jsonrpc_mstream_t *mstream)
{
	return mstream->stream;
}


