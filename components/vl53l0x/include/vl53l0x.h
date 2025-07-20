#ifndef VL53L0X_H
#define VL53L0X_H

#include <driver/i2c.h>
#include <esp_err.h>

#ifdef __cplusplus
extern "C" {
#endif

#define VL53L0X_I2C_ADDR 0x29

esp_err_t vl53l0x_init(i2c_port_t port);
int vl53l0x_read_range_mm(i2c_port_t port);

#ifdef __cplusplus
}
#endif

#endif // VL53L0X_H
