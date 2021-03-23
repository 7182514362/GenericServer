project(reactor_test)

set(CMAKE_BUILD_TYPE "Debug")


include_directories(${PROJECT_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR}/src/log)

add_executable(reactor_test reactor_test.cpp)
target_link_libraries(reactor_test base log)



