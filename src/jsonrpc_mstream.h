//
//  jsonrpc_mstream.h
//  jsonrpc
//
//

#ifndef jsonrpc_jsonrpc_mstream_h
#define jsonrpc_jsonrpc_mstream_h

#include <stdio.h>
#include <stdarg.h>

typedef struct jsonrpc_mstream	jsonrpc_mstream_t;

jsonrpc_mstream_t *
jsonrpc_mstream_open (void);

void
jsonrpc_mstream_close (jsonrpc_mstream_t *mstream);

int
jsonrpc_mstream_vprint (jsonrpc_mstream_t *mstream, const char *fmt, va_list ap);

int
jsonrpc_mstream_print (jsonrpc_mstream_t *mstream, const char *fmt, ...);

size_t
jsonrpc_mstream_length (jsonrpc_mstream_t *mstream);

void
jsonrpc_mstream_rewind (jsonrpc_mstream_t *mstream);

const char *
jsonrpc_mstream_getbuf (jsonrpc_mstream_t *mstream);

#endif
