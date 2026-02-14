cmake_minimum_required ( VERSION 3.26 )

configure_file (
    "${CMAKE_CURRENT_SOURCE_DIR}/include/misc/CMakeVars.hpp.in"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/misc/CMakeVars.hpp"
)

if ( ENABLE_LINTER )
    set ( BUILD_OPTIONS ENABLE_LINTER )
endif()

if ( "${CMAKE_SYSTEM_NAME}" STREQUAL "Android" )
make_static_library ( ${LIB_TARGET_NAME} ${BUILD_OPTIONS}
    DEPS
        nlohmann_json::nlohmann_json
        dgm::dgm-lib
        #fsm::fsm-lib
        TGUI::TGUI
        SFML::Audio
        fmt::fmt
        range-v3::range-v3
)
else ()
make_static_library ( ${LIB_TARGET_NAME} ${BUILD_OPTIONS}
    DEPS
        nlohmann_json::nlohmann_json
        dgm::dgm-lib
        #fsm::fsm-lib
        TGUI::TGUI
        SFML::Audio
)
endif ()

if (UNIX)
	target_compile_definitions( ${LIB_TARGET_NAME} PRIVATE LINUX=1 )
endif()

target_precompile_headers( ${LIB_TARGET_NAME}
    PUBLIC
        <vector>
        <string>
        <algorithm>
        <functional>
        <utility>
        <variant>
        <tuple>
        <iostream>
        <fstream>
        <map>
        <unordered_map>
        <set>
        <nlohmann/json.hpp>
)
