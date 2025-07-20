#include "vl53l0x.h"
#include <string.h>
#include <stdlib.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

static esp_err_t write_multi(i2c_port_t port, uint16_t reg, const uint8_t *data, size_t len)
{
    uint8_t buf[len + 2];
    buf[0] = reg >> 8;
    buf[1] = reg & 0xFF;
    memcpy(buf + 2, data, len);
    return i2c_master_write_to_device(port, VL53L0X_I2C_ADDR, buf, len + 2, pdMS_TO_TICKS(100));
}

static esp_err_t write_reg(i2c_port_t port, uint16_t reg, uint8_t val)
{
    return write_multi(port, reg, &val, 1);
}

static esp_err_t read_reg(i2c_port_t port, uint16_t reg, uint8_t *val)
{
    uint8_t regbuf[2] = { reg >> 8, reg & 0xFF };
    return i2c_master_write_read_device(port, VL53L0X_I2C_ADDR, regbuf, sizeof(regbuf), val, 1, pdMS_TO_TICKS(100));
}

static esp_err_t read_reg16(i2c_port_t port, uint16_t reg, uint16_t *val)
{
    uint8_t regbuf[2] = { reg >> 8, reg & 0xFF };
    uint8_t data[2];
    esp_err_t err = i2c_master_write_read_device(port, VL53L0X_I2C_ADDR, regbuf, sizeof(regbuf), data, 2, pdMS_TO_TICKS(100));
    if (err == ESP_OK) {
        *val = ((uint16_t)data[0] << 8) | data[1];
    }
    return err;
}

#include "core/inc/vl53l0x_tuning.h"

esp_err_t vl53l0x_init(i2c_port_t port)
{
    // Load default tuning settings from ST header
    for (size_t i = 0; i + 2 < sizeof(DefaultTuningSettings); i += 3) {
        uint8_t type = DefaultTuningSettings[i];
        uint8_t reg_high = DefaultTuningSettings[i + 1];
        uint8_t val = DefaultTuningSettings[i + 2];
        if (type == 0 && reg_high == 0 && val == 0) {
            break;
        }
        uint16_t reg = ((uint16_t)type << 8) | reg_high;
        esp_err_t err = write_reg(port, reg, val);
        if (err != ESP_OK) {
            return err;
        }
    }

    // start continuous ranging
    return write_reg(port, 0x000, 0x01);
}

int vl53l0x_read_range_mm(i2c_port_t port)
{
    uint8_t status = 0;
    int timeout = 20;
    while (timeout--) {
        if (read_reg(port, 0x0013, &status) != ESP_OK) {
            return -1;
        }
        if (status & 0x07) {
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    if (!(status & 0x07)) {
        return -1;
    }

    uint16_t range = 0;
    if (read_reg16(port, 0x0014 + 10, &range) != ESP_OK) {
        return -1;
    }

    write_reg(port, 0x0B, 0x01); // clear interrupt
    return range;
}
