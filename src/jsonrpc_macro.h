//
//  jsonrpc_macro.h
//  jsonrpc
//
//

#ifndef jsonrpc_jsonrpc_macro_h
#define jsonrpc_jsonrpc_macro_h

#define JSONRPC_DEBUG

#define	JSONRPC_VERSION			"2.0"
#define	JSONRPC_PRIVATE			static
#define	JSONRPC_API
#define	JSONRPC_STRNCPY(d,s,n)	do{strncpy(d,s,n); d[n-1] = '\0';}while(0)

#ifdef JSONRPC_DEBUG
#define JSONRPC_THROW(cond, expr)	\
	if (cond) {\
		fprintf(stderr, "%s<%d>:exception:%s\n", __FUNCTION__, __LINE__, #cond);\
		expr;\
	} else {}
#else
#define JSONRPC_THROW(cond, expr)	\
	if (cond) {\
		expr;\
	} else {}
#endif

#if defined(WIN32) || defined(_WIN32)
#define	snprintf		_snprintf
#define	vsnprintf		_vsnprintf
#define	strtok_r		strtok_s
#endif

#endif
