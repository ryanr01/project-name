#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c_master.h"
#include "esp_log.h"
#include "vl53l0x.h"

#define I2C_MASTER_SCL_IO 19
#define I2C_MASTER_SDA_IO 18
#define I2C_MASTER_PORT 0
#define I2C_MASTER_FREQ_HZ 400000

static const char *TAG = "MAIN";

static void i2c_master_init(i2c_master_bus_handle_t *bus, i2c_master_dev_handle_t *dev)
{
    i2c_master_bus_config_t bus_config = {
        .i2c_port = I2C_MASTER_PORT,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags = { .enable_internal_pullup = true },
    };
    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_config, bus));

    i2c_device_config_t dev_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = VL53L0X_I2C_ADDR,
        .scl_speed_hz = I2C_MASTER_FREQ_HZ,
    };
    ESP_ERROR_CHECK(i2c_master_bus_add_device(*bus, &dev_config, dev));
}

void app_main(void)
{
    i2c_master_bus_handle_t bus;
    i2c_master_dev_handle_t dev;
    i2c_master_init(&bus, &dev);
    ESP_ERROR_CHECK(vl53l0x_init(dev));

    while (1) {
        int dist = vl53l0x_read_range_mm(dev);
        if (dist >= 0) {
            ESP_LOGI(TAG, "Distance: %d mm", dist);
        } else {
            ESP_LOGE(TAG, "Read failed");
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
