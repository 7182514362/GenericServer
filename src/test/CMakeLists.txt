
set(CMAKE_BUILD_TYPE "Debug")


include_directories(${PROJECT_SOURCE_DIR}/src
                    ${PROJECT_SOURCE_DIR}/src/log
                    ${PROJECT_SOURCE_DIR}/src/reactor
                    ${PROJECT_SOURCE_DIR}/src/net)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__FILENAME__='\"$(notdir $(abspath $<))\"'")

add_executable(TCPServer_test TCPServer_test.cpp)
add_executable(TCPClient_test TCPClient_test.cpp)
add_executable(reactor_test reactor_test.cpp)
add_executable(AsyncLogger_test AsyncLogger_test.cpp)
add_executable(SyncLogger_test SyncLogger_test.cpp)

target_link_libraries(TCPServer_test base log reactor net)
target_link_libraries(TCPClient_test base log reactor net)
target_link_libraries(reactor_test base log)
target_link_libraries(AsyncLogger_test base log)
target_link_libraries(SyncLogger_test base log)



