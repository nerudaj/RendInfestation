cmake_minimum_required ( VERSION 3.26 )

make_executable ( navmesh-debugger DEPS ${LIB_TARGET_NAME} )

set_target_properties ( navmesh-debugger PROPERTIES
	VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
    WIN32_EXECUTABLE ON
)
