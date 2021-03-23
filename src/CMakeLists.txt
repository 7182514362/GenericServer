set(SRCS
BlockingQueue.cpp
Buffer.cpp
CurrentThread.cpp
Util.cpp
Time.cpp
)

set(CMAKE_BUILD_TYPE "Debug")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__FILENAME__='\"$(notdir $(abspath $<))\"'")

add_library(base ${SRCS})
target_link_libraries(base pthread)

add_subdirectory(log)
add_subdirectory(reactor)
add_subdirectory(net)

