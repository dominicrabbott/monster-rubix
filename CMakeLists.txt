cmake_minimum_required(VERSION 2.8)

project(MonsterRubix)

find_package(OGRE REQUIRED COMPONENTS Bites)
find_package(Boost REQUIRED COMPONENTS filesystem iostreams)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
add_subdirectory(src)
file(COPY ${OGRE_CONFIG_DIR}/resources.cfg DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources DESTINATION ${CMAKE_BINARY_DIR})
