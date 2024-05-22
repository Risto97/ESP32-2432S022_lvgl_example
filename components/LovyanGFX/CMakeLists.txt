# CMakeLists for ESP-IDF
set(LOVYANGFX_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/../../deps/_deps/lovyangfx-src/)

file(GLOB SRCS
     ${LOVYANGFX_SOURCE_DIR}/src/lgfx/Fonts/efont/*.c
     ${LOVYANGFX_SOURCE_DIR}/src/lgfx/Fonts/IPA/*.c
     ${LOVYANGFX_SOURCE_DIR}/src/lgfx/utility/*.c
     ${LOVYANGFX_SOURCE_DIR}/src/lgfx/v0/*.cpp
     ${LOVYANGFX_SOURCE_DIR}/src/lgfx/v0/panel/*.cpp
     ${LOVYANGFX_SOURCE_DIR}/src/lgfx/v0/platforms/*.cpp
     ${LOVYANGFX_SOURCE_DIR}/src/lgfx/v0/touch/*.cpp
     ${LOVYANGFX_SOURCE_DIR}/src/lgfx/v1/*.cpp
     ${LOVYANGFX_SOURCE_DIR}/src/lgfx/v1/misc/*.cpp
     ${LOVYANGFX_SOURCE_DIR}/src/lgfx/v1/panel/*.cpp
     ${LOVYANGFX_SOURCE_DIR}/src/lgfx/v1/platforms/arduino_default/*.cpp
     ${LOVYANGFX_SOURCE_DIR}/src/lgfx/v1/platforms/esp32/*.cpp
     ${LOVYANGFX_SOURCE_DIR}/src/lgfx/v1/platforms/esp32c3/*.cpp
     ${LOVYANGFX_SOURCE_DIR}/src/lgfx/v1/platforms/esp32s2/*.cpp
     ${LOVYANGFX_SOURCE_DIR}/src/lgfx/v1/platforms/esp32s3/*.cpp
     ${LOVYANGFX_SOURCE_DIR}/src/lgfx/v1/touch/*.cpp
     )

if (IDF_VERSION_MAJOR GREATER_EQUAL 5)
    set(COMPONENT_REQUIRES nvs_flash efuse esp_lcd driver esp_timer)
elseif ((IDF_VERSION_MAJOR EQUAL 4) AND (IDF_VERSION_MINOR GREATER 3) OR IDF_VERSION_MAJOR GREATER 4)
    set(COMPONENT_REQUIRES nvs_flash efuse esp_lcd)
else()
    set(COMPONENT_REQUIRES nvs_flash efuse)
endif()


### If you use arduino-esp32 components, please activate next comment line.
# list(APPEND COMPONENT_REQUIRES arduino-esp32)

message(STATUS "LovyanGFX use components = ${COMPONENT_REQUIRES}")

idf_component_register(SRCS ${SRCS}
                       INCLUDE_DIRS ${LOVYANGFX_SOURCE_DIR}/src
                       REQUIRES ${COMPONENT_REQUIRES}
                       )

target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-uninitialized)
