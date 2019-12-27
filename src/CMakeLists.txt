include_directories(${MonsterRubix_SOURCE_DIR}/include)
add_executable(MonsterRubix main.cpp color.cpp face.cpp cube_manipulator.cpp ui_manager.cpp cube_display.cpp keyboard_ui_manager.cpp cube.cpp cube_centers.cpp cube_base.cpp three_cube_solver.cpp ai_ui_manager.cpp center_solver.cpp edge_solver.cpp twist_utils.cpp)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
target_link_libraries(MonsterRubix OgreBites boost_serialization boost_filesystem boost_system)
