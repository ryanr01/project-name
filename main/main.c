#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include "vl53l0x.h"

#define I2C_MASTER_SCL_IO 19
#define I2C_MASTER_SDA_IO 18
#define I2C_MASTER_NUM I2C_NUM_0
#define I2C_MASTER_FREQ_HZ 400000

static const char *TAG = "MAIN";

static esp_err_t i2c_master_init(void)
{
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };
    ESP_ERROR_CHECK(i2c_param_config(I2C_MASTER_NUM, &conf));
    return i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0);
}

void app_main(void)
{
    ESP_ERROR_CHECK(i2c_master_init());
    ESP_ERROR_CHECK(vl53l0x_init(I2C_MASTER_NUM));

    while (1) {
        int dist = vl53l0x_read_range_mm(I2C_MASTER_NUM);
        if (dist >= 0) {
            ESP_LOGI(TAG, "Distance: %d mm", dist);
        } else {
            ESP_LOGE(TAG, "Read failed");
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
