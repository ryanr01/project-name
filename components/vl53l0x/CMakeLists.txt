idf_component_register(SRCS "vl53l0x.c"
                       INCLUDE_DIRS "include" "core/inc"
                       REQUIRES "driver" "freertos" "nvs_flash")
