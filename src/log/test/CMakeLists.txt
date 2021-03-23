#project(test)

set(CMAKE_BUILD_TYPE "Debug")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__FILENAME__='\"$(notdir $(abspath $<))\"'")
include_directories(${PROJECT_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR}/src/log)

add_executable(AsyncLogger_test AsyncLogger_test.cpp)
target_link_libraries(AsyncLogger_test base log)

add_executable(SyncLogger_test SyncLogger_test.cpp)
target_link_libraries(SyncLogger_test base log)


