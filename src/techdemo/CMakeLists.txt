set(SRC_FILES
    Main.cpp)

add_executable(techdemo ${SRC_FILES})
set_target_properties(techdemo PROPERTIES FOLDER apps)

target_link_libraries(techdemo wlf_render)