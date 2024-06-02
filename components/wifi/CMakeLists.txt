set(srcs 
         wifi_utils.c
         http_utils.cpp
         )

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS .
                       PRIV_REQUIRES esp_netif driver esp_wifi vfs
                       REQUIRES json smart_fridge
                       )
