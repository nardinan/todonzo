cmake_minimum_required(VERSION 3.16)

project("todonzo" C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
execute_process(COMMAND sdl2-config --libs OUTPUT_VARIABLE STATUS_SDL_LIBRARY)
execute_process(COMMAND sdl2-config --cflags OUTPUT_VARIABLE STATUS_SDL_INCLUDE)
string(REGEX REPLACE "\n$" "" STATUS_SDL_LIBRARY "${STATUS_SDL_LIBRARY}")
string(REGEX REPLACE "\n$" "" STATUS_SDL_INCLUDE "${STATUS_SDL_INCLUDE}")

set(CMAKE_C_FLAGS "-std=gnu99 ${CMAKE_C_FLAGS}")
add_executable(todonzo
  src/todonzo.c
  src/reminder.c
  src/reminder.h
  src/array.c
  src/array.h
  src/notification.c
  src/notification.h
  src/xdg.c
  src/xdg.h
  src/lock.c
  src/lock.h)

INCLUDE(FindPkgConfig)
PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
PKG_SEARCH_MODULE(SDL2TTF REQUIRED SDL2_ttf)
PKG_SEARCH_MODULE(SDL2IMAGE REQUIRED SDL2_image)

INCLUDE_DIRECTORIES(${STATUS_SDL_INCLUDE} ${SDL2_INCLUDE_DIRS} ${SDL2TTF_INCLUDE_DIRS}
  ${SDL2IMAGE_INCLUDE_DIRS})

target_link_libraries(todonzo ${STATUS_SDL_LIBRARY} ${SDL2_LIBRARIES} ${SDL2TTF_LIBRARIES}
  ${SDL2IMAGE_LIBRARIES} m)

install(TARGETS todonzo DESTINATION bin)
install(DIRECTORY todonzo DESTINATION share)
