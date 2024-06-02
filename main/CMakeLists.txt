set(SOURCES 
        main.cpp
        CST820.cpp
        gui_config.cpp
    )
idf_component_register(SRCS ${SOURCES}
                    INCLUDE_DIRS .
                    REQUIRES esp_event esp_timer driver LovyanGFX lvgl wifi smart_fridge
                    )

target_compile_definitions(${COMPONENT_LIB} PUBLIC LV_CONF_INCLUDE_SIMPLE=1 )
target_compile_options(${COMPONENT_LIB} PUBLIC -O3)

