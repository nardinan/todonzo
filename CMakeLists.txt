cmake_minimum_required(VERSION 3.16)
project(todonzo C)

set(CMAKE_C_STANDARD 11)

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

find_package(PkgConfig REQUIRED)
pkg_check_modules(NOTIFY REQUIRED libnotify)
target_link_libraries(todonzo ${NOTIFY_LIBRARIES})
target_link_libraries(todonzo m)
target_include_directories(todonzo PUBLIC ${NOTIFY_INCLUDE_DIRS})
target_compile_options(todonzo PUBLIC ${NOTIFY_CFLAGS_OTHER})

install(TARGETS todonzo DESTINATION bin)
install(DIRECTORY todonzo DESTINATION share)
