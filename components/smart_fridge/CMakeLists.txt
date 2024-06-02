idf_component_register(SRCS 
                           app.cpp
                           barcode_scan.c
                           stock_tab.cpp
                       INCLUDE_DIRS .
                       REQUIRES lvgl
                       )

target_compile_options(${COMPONENT_LIB} PUBLIC -O3)

