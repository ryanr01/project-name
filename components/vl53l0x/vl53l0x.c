#include "vl53l0x.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#define TAG "VL53L0X"

static esp_err_t write_reg(i2c_port_t port, uint8_t reg, uint8_t value)
{
    uint8_t data[2] = {reg, value};
    return i2c_master_write_to_device(port, VL53L0X_I2C_ADDR, data, sizeof(data), pdMS_TO_TICKS(100));
}

static esp_err_t read_reg(i2c_port_t port, uint8_t reg, uint8_t *value)
{
    return i2c_master_write_read_device(port, VL53L0X_I2C_ADDR, &reg, 1, value, 1, pdMS_TO_TICKS(100));
}

static esp_err_t read_reg16(i2c_port_t port, uint8_t reg, uint16_t *value)
{
    uint8_t buf[2];
    esp_err_t err = i2c_master_write_read_device(port, VL53L0X_I2C_ADDR, &reg, 1, buf, 2, pdMS_TO_TICKS(100));
    if (err == ESP_OK) {
        *value = ((uint16_t)buf[0] << 8) | buf[1];
    }
    return err;
}

esp_err_t vl53l0x_init(i2c_port_t port)
{
    // simple check for device presence
    uint8_t id = 0;
    esp_err_t err = read_reg(port, 0xC0, &id); // IDENTIFICATION_MODEL_ID
    if (err != ESP_OK) {
        return err;
    }
    if (id != 0xEE) {
        ESP_LOGE(TAG, "Unexpected model id: 0x%02x", id);
        return ESP_ERR_INVALID_RESPONSE;
    }

    // Set I2C standard mode
    err |= write_reg(port, 0x88, 0x00);

    // minimal init sequence based on ST API
    err |= write_reg(port, 0x80, 0x01);
    err |= write_reg(port, 0xFF, 0x01);
    err |= write_reg(port, 0x00, 0x00);
    uint8_t stop_variable;
    read_reg(port, 0x91, &stop_variable);
    err |= write_reg(port, 0x00, 0x01);
    err |= write_reg(port, 0xFF, 0x00);
    err |= write_reg(port, 0x80, 0x00);
    vTaskDelay(pdMS_TO_TICKS(10));
    return err;
}

int vl53l0x_read_range_mm(i2c_port_t port)
{
    esp_err_t err = write_reg(port, 0x00, 0x01); // SYSRANGE_START
    if (err != ESP_OK) {
        return -1;
    }
    // wait for start bit to clear
    uint8_t status;
    do {
        err = read_reg(port, 0x00, &status);
        if (err != ESP_OK) return -1;
    } while (status & 0x01);

    // polling for data ready
    uint8_t ready = 0;
    int timeout = 20;
    while (!(ready & 0x07) && timeout--) {
        err = read_reg(port, 0x13, &ready); // RESULT_INTERRUPT_STATUS
        if (err != ESP_OK) return -1;
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    if (!(ready & 0x07)) {
        ESP_LOGE(TAG, "timeout waiting for data ready");
        return -1;
    }

    uint16_t range;
    err = read_reg16(port, 0x14 + 10, &range); // RESULT_RANGE_STATUS + 10
    if (err != ESP_OK) return -1;

    write_reg(port, 0x0B, 0x01); // SYSTEM_INTERRUPT_CLEAR
    return range;
}
