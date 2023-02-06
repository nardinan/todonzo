cmake_minimum_required(VERSION 3.24)
project(todonzo C)

set(CMAKE_C_STANDARD 11)

add_executable(todonzo src/todonzo.c src/reminder.c src/reminder.h src/array.c src/array.h src/notification.c src/notification.h)
