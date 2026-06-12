include ( "${CMAKE_CURRENT_LIST_DIR}/add-catch.cmake" )

set ( DGM_ENGINE_CORE_LIB_VERSION "rend-infestation" )
set ( CXXOPTS_VERSION "3.3.1" )
set ( FSM_LIB_VERSION "main" )
set ( CATCH2_VERSION "v3.8.0" )
set ( ENTT_VERSION "3.16.0" )

CPMAddPackage("gh:nerudaj/dgm-engine-core-lib#${DGM_ENGINE_CORE_LIB_VERSION}")
CPMAddPackage("gh:jarro2783/cxxopts#v${CXXOPTS_VERSION}")
CPMAddPackage("gh:nlohmann/json#v${NLOHMANN_VERSION}")
CPMAddPackage("gh:skypjack/entt@${ENTT_VERSION}")
CPMAddPackage("gh:nerudaj/fsm-lib#${FSM_LIB_VERSION}") 
AddCatch( "${CATCH2_VERSION}" )
