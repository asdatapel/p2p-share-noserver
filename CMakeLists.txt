cmake_minimum_required(VERSION 3.6)
project(p2p_share)

set(CMAKE_CXX_STANDARD 11)


include_directories(ext/SFML/include)

link_directories(ext/SFML/lib)

set(SOURCE_FILES main.cpp IndexServer.cpp IndexServer.h Connection.cpp Connection.h Client.cpp Client.h)
add_executable(p2p_share ${SOURCE_FILES})
target_link_libraries(p2p_share sfml-system.a)
target_link_libraries(p2p_share sfml-network.a)

add_executable(p2p_Client ${SOURCE_FILES})
target_link_libraries(p2p_Client sfml-system.a)
target_link_libraries(p2p_Client sfml-network.a)
target_compile_definitions(p2p_Client PRIVATE SERVER=0)

add_executable(p2p_Server ${SOURCE_FILES})
target_link_libraries(p2p_Server sfml-system.a)
target_link_libraries(p2p_Server sfml-network.a)
target_compile_definitions(p2p_Server PRIVATE SERVER=1)
