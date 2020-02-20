include_directories(${MonsterRubix_SOURCE_DIR}/include)
add_executable(MonsterRubix main.cpp color.cpp face.cpp ui_manager.cpp cube_display.cpp keyboard_ui_manager.cpp cube.cpp cube_centers.cpp cube_base.cpp three_cube_solver.cpp center_solver.cpp edge_solver.cpp twist_utils.cpp cube_solver.cpp multi_cube_ui.cpp)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
target_link_libraries(MonsterRubix OgreBites boost_filesystem boost_system boost_iostreams)
