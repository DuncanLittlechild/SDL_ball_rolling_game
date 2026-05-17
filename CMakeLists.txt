cmake_minimum_required(VERSION 4.2)
project(SDL_ball_rolling_game)

set(CMAKE_CXX_STANDARD 23)

include_directories(imgui)
include_directories(phases)
include_directories(structs)
include_directories(utils)

find_package(SDL3 REQUIRED)
find_package(SDL3_image REQUIRED)

add_executable(SDL_ball_rolling_game
        phases/phases.cpp
        structs/project_structs.cpp
        utils/dl_imgui_functions.cpp
        main.cpp
        utils/project_utils.cpp
        utils/project_utils.h
)

target_link_libraries(SDL_ball_rolling_game PRIVATE
        SDL3::SDL3
        SDL3_image::SDL3_image
        ${CMAKE_SOURCE_DIR}/imgui/libimgui.a
)
