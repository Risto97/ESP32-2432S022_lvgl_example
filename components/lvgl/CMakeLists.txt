set(LVGL_SOURCE_DIR ../../deps/_deps/lvgl-src/)
file(GLOB_RECURSE SOURCES ${LVGL_SOURCE_DIR}/src/*.c
                          ${LVGL_SOURCE_DIR}/demos/widgets/assets/*.c
                          ${LVGL_SOURCE_DIR}/demos/*.c
                          ${LVGL_SOURCE_DIR}/examples/*.c
                          )

idf_component_register(SRCS ${SOURCES}
                       INCLUDE_DIRS ${LVGL_SOURCE_DIR} . ${LVGL_SOURCE_DIR}/examples/ ${LVGL_SOURCE_DIR}/demos/
                       )

target_compile_definitions(${COMPONENT_LIB} PUBLIC LV_CONF_INCLUDE_SIMPLE=1)

target_compile_options(${COMPONENT_LIB} PUBLIC -O3)

