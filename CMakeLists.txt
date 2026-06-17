cmake_minimum_required(VERSION 3.24)
project(SDL_ball_rolling_game)

set(CMAKE_CXX_STANDARD 23)

include(FetchContent)

# --- SDL3 ---
FetchContent_Declare(SDL3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG        main
)
FetchContent_MakeAvailable(SDL3)

# --- SDL3_image ---
FetchContent_Declare(SDL3_image
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
        GIT_TAG        main
)
FetchContent_MakeAvailable(SDL3_image)

# --- SDL3_ttf
FetchContent_Declare(SDL3_ttf
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git
        GIT_TAG        main
)
FetchContent_MakeAvailable(SDL3_ttf)

# --- Dear ImGui (compiled inline, no prebuilt lib needed) ---
FetchContent_Declare(imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG        master
)
FetchContent_MakeAvailable(imgui)

add_library(imgui_lib STATIC
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer3.cpp
        # Add/remove backends to match what you actually use
)
target_include_directories(imgui_lib PUBLIC
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
)
target_link_libraries(imgui_lib PUBLIC SDL3::SDL3)

# --- Your executable ---
add_executable(SDL_ball_rolling_game
        phases/phases.cpp
        structs/project_structs.cpp
        utils/dl_imgui_functions.cpp
        utils/project_utils.cpp
        main.cpp
)

target_include_directories(SDL_ball_rolling_game PRIVATE
        phases structs utils
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
)

target_link_libraries(SDL_ball_rolling_game PRIVATE
        SDL3::SDL3
        SDL3_image::SDL3_image
        SDL3_ttf::SDL3_ttf
        imgui_lib
)