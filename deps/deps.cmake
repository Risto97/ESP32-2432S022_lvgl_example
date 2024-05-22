set(FETCHCONTENT_BASE_DIR ${CMAKE_CURRENT_LIST_DIR}/_deps CACHE INTERNAL "")
set(CPM_DOWNLOAD_VERSION 0.38.1)
include(${CMAKE_CURRENT_LIST_DIR}/CPM.cmake)

CPMAddPackage(
    NAME lvgl 
    VERSION 8.4.0
    GIT_REPOSITORY "https://github.com/lvgl/lvgl.git"
    # OPTIONS "LV_CONF_BUILD_DISABLE_DEMOS=OFF;LV_CONF_BUILD_DISABLE_EXAMPLES=OFF"
    DOWNLOAD_ONLY YES
    )

CPMAddPackage(
    NAME LovyanGFX 
    GIT_TAG 1.1.2
    GIT_REPOSITORY "https://github.com/lovyan03/LovyanGFX.git"
    # OPTIONS "LV_CONF_BUILD_DISABLE_DEMOS=OFF;LV_CONF_BUILD_DISABLE_EXAMPLES=OFF"
    DOWNLOAD_ONLY YES
    )
