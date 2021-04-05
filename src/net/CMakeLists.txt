set(SRCS
InetAddress.cpp
Acceptor.cpp
Socket.cpp
IdleConnections.cpp
TCPConnection.cpp
TCPServer.cpp
TCPClient.cpp
Connector.cpp
)

set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__FILENAME__='\"$(notdir $(abspath $<))\"'")

include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/src/log)
include_directories(${PROJECT_SOURCE_DIR}/src/reactor)

add_library(net ${SRCS})
target_link_libraries(net base log reactor)