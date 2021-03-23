
set(CMAKE_BUILD_TYPE "Debug")

include_directories(${PROJECT_SOURCE_DIR}/src
${PROJECT_SOURCE_DIR}/src/log
${PROJECT_SOURCE_DIR}/src/reactor
${PROJECT_SOURCE_DIR}/src/net
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__FILENAME__='\"$(notdir $(abspath $<))\"'")

add_executable(EchoServer_test EchoServer_test.cpp)

target_link_libraries(EchoServer_test base log reactor net)


