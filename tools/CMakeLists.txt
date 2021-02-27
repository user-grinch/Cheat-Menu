################################################################################
# Build Tools
################################################################################

cmake_minimum_required(VERSION 3.0)
project(CheatMenuInjector)

add_executable(CheatMenuInjector WIN32 "Injector.cpp")

set_target_properties(CheatMenuInjector PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY  "${GTA_SA_DIR}/$<0:>/"
)

target_link_options(${PROJECT_NAME} PRIVATE
    /SUBSYSTEM:CONSOLE
)