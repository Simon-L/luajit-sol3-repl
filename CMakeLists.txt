cmake_minimum_required(VERSION 3.14)

include(cmake/prelude.cmake)

project(
    soljit
    VERSION 0.1.0
    DESCRIPTION "Short description"
    HOMEPAGE_URL "https://example.com/"
    LANGUAGES CXX
)

include(cmake/project-is-top-level.cmake)
include(cmake/variables.cmake)

# ---- Declare library ----

# add_library(
#     soljit_lib OBJECT
#     source/lib.cpp
# )
#
# target_include_directories(
#     soljit_lib ${warning_guard}
#     PUBLIC
#     "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/source>"
# )
#
# target_compile_features(soljit_lib PUBLIC cxx_std_17)

add_custom_target(hererocks
  COMMAND hererocks -j @224129a -rlatest --verbose ${CMAKE_BINARY_DIR}
  COMMAND ${CMAKE_BINARY_DIR}/bin/luarocks build luarepl
  COMMAND ${CMAKE_BINARY_DIR}/bin/luarocks build linenoise
  COMMAND ${CMAKE_BINARY_DIR}/bin/luarocks build lua-term
  COMMENT "Build luajit and luarocks with hererocks!"
)

# ---- Declare executable ----

add_executable(soljit_exe source/main.cpp libs/cpp-base64/base64.cpp)
add_executable(soljit::exe ALIAS soljit_exe)

list(APPEND CMAKE_PREFIX_PATH ${PROJECT_SOURCE_DIR}/prefix)
find_package(nng)

set(JSON_BuildTests OFF CACHE INTERNAL "")
set(JSON_Install OFF CACHE INTERNAL "")
add_subdirectory(libs/json)
target_link_libraries(soljit_exe PRIVATE nlohmann_json::nlohmann_json)

add_subdirectory(libs/nngpp)
target_link_libraries(soljit_exe PRIVATE nngpp)
target_include_directories(soljit_exe PUBLIC "${PROJECT_SOURCE_DIR}/libs/nngpp/include")
target_include_directories(soljit_exe PUBLIC "${PROJECT_SOURCE_DIR}/libs/cpp-base64")

add_subdirectory(libs/cxxopts)
target_link_libraries(soljit_exe PRIVATE cxxopts)

set_property(TARGET soljit_exe PROPERTY OUTPUT_NAME soljit)

target_compile_features(soljit_exe PRIVATE cxx_std_17)

# target_link_libraries(soljit_exe PRIVATE soljit_lib)

target_include_directories(soljit_exe PUBLIC "${CMAKE_BINARY_DIR}/include")
add_subdirectory(libs/sol2)
target_link_libraries(soljit_exe PRIVATE sol2)

set_target_properties(soljit_exe PROPERTIES BUILD_RPATH "./lib")
target_link_libraries(soljit_exe PUBLIC ":libluajit-5.1.so.2")

# add_custom_target(run ALL
add_custom_target(run
  COMMAND ./${PROJECT_NAME}
  COMMENT "run ${PROJECT_NAME}"
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  VERBATIM
  DEPENDS soljit_exe
)

# ---- Install rules ----

if(NOT CMAKE_SKIP_INSTALL_RULES)
  include(cmake/install-rules.cmake)
endif()

# ---- Developer mode ----

if(NOT soljit_DEVELOPER_MODE)
  return()
elseif(NOT PROJECT_IS_TOP_LEVEL)
  message(
      AUTHOR_WARNING
      "Developer mode is intended for developers of soljit"
  )
endif()

include(cmake/dev-mode.cmake)

# ---- Run ----
