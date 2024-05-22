cmake_minimum_required(VERSION 3.5)

add_compile_options(-Wformat=0)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
include(deps/deps.cmake)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

if (NOT DEFINED PROJECT_NAME)
	project(test_lvgl)
endif (NOT DEFINED PROJECT_NAME)

