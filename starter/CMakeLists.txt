add_executable(starter WIN32 starter.cpp starter_res.rc)
set_target_properties(starter PROPERTIES OUTPUT_NAME "${STARTER_NAME}")
target_link_libraries(starter PRIVATE hackKit_static shlwapi psapi)
