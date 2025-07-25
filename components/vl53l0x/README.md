# VL53L0X component

This component provides a minimal driver for the VL53L0X time-of-flight sensor using the ESP-IDF I2C master driver. It exposes two functions:

```c
esp_err_t vl53l0x_init(i2c_port_t port);
int vl53l0x_read_range_mm(i2c_port_t port);
```

`vl53l0x_init` performs a basic initialization sequence for the sensor. `vl53l0x_read_range_mm` triggers a single measurement and returns the distance in millimetres, or `-1` on error.

The component is automatically included when building the example project in this repository.
