set(log_srcs
Appender.cpp
FileUtil.cpp
Log.cpp
LogFile.cpp
LogStream.cpp
RingBuffer.cpp
)

set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__FILENAME__='\"$(notdir $(abspath $<))\"'")

include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/src/log)

add_library(log ${log_srcs})
target_link_libraries(log base)

add_subdirectory(test)