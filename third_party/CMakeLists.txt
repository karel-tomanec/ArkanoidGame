# SDL
include(FetchContent)
FetchContent_Declare(
    SDL
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-3.2.14
)
FetchContent_MakeAvailable(SDL)

add_subdirectory(SDL3_ttf)