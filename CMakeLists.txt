cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 11)

include(cmake/HunterGate.cmake)

HunterGate(
    URL "https://github.com/uliana99/hunter/archive/v0.19.137.1.tar.gz"
    SHA1 "87af3ced13dd7a194ca94db4cd0d21908d80b596"
)

project(json)

project(process)

hunter_add_package(Boost COMPONENTS system filesystem program_options)
find_package(Boost CONFIG REQUIRED system filesystem program_options)

add_executable(builder ${CMAKE_CURRENT_SOURCE_DIR}/sources/builder.cpp)
target_link_libraries(builder Boost::system Boost::filesystem Boost::program_options)

install(TARGETS builder RUNTIME DESTINATION bin)
