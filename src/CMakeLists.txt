

SET (SRCS 
		jsonrpc_server.c
		jsonrpc_mstream.c
		jsonrpc_memory.c
)
SET (HDRS
		jsonrpc_memory.h
		jsonrpc_mstream.h
)
SET (PUBH
		jsonrpc.h
)

SET (LIB_DIR ${CMAKE_CURRENT_BINARY_DIR}/../${JSONRPC_DIST_NAME}/lib)
SET (INC_DIR ${CMAKE_CURRENT_BINARY_DIR}/../${JSONRPC_DIST_NAME}/include)

SET (LIBRARY_OUTPUT_PATH ${LIB_DIR})

ADD_LIBRARY (jsonrpc_s STATIC ${SRCS} ${HDRS} ${PUBH})
ADD_LIBRARY (jsonrpc SHARED ${SRCS} ${HDRS} ${PUBH})

SET_TARGET_PROPERTIES(jsonrpc PROPERTIES
                      DEFINE_SYMBOL JSONRPC_SHARED
                      SOVERSION ${JSONRPC_VERSION_MAJOR}
                      VERSION ${JSONRPC_VERSION_MAJOR}.${JSONRPC_VERSION_MINOR}
)

IF(APPLE)
  MESSAGE("INSTALL_NAME_DIR: ${CMAKE_INSTALL_PREFIX}/lib")
  SET_TARGET_PROPERTIES(jsonrpc PROPERTIES
                        INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib")
ENDIF(APPLE)

FILE(MAKE_DIRECTORY ${LIB_DIR})
FILE(MAKE_DIRECTORY ${INC_DIR})

CONFIGURE_FILE(jsonrpc_config.h.in ${INC_DIR}/jsonrpc_config.h)

FOREACH (header ${PUBH})
  SET (header ${CMAKE_CURRENT_SOURCE_DIR}/${header})

  EXEC_PROGRAM(${CMAKE_COMMAND} ARGS -E copy_if_different ${header} ${INC_DIR})

  ADD_CUSTOM_COMMAND(TARGET jsonrpc_s POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different ${header} ${INC_DIR})
ENDFOREACH (header ${PUBH})

INCLUDE_DIRECTORIES(${INC_DIR})


