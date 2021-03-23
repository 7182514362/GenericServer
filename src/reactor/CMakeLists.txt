set(SRCS
Event.cpp
EventLoop.cpp
EventLoopThread.cpp
EventLoopThreadPool.cpp
Poller.cpp
SystemTimer.cpp
Timer.cpp
)

set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__FILENAME__='\"$(notdir $(abspath $<))\"'")

include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/src/net)
include_directories(${PROJECT_SOURCE_DIR}/src/log)

add_library(reactor ${SRCS})
target_link_libraries(reactor base log)

#add_subdirectory(test)