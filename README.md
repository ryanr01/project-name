# _Sample project_

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This is the simplest buildable example. The example is used by command `idf.py create-project`
that copies the project to user specified path and set it's name. For more information follow the [docs page](https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html#start-a-new-project)



## How to use example
We encourage the users to use the example as a template for the new projects.
A recommended way is to follow the instructions on a [docs page](https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html#start-a-new-project).

## Example folder contents

The project **sample_project** contains one source file in C language [main.c](main/main.c). The file is located in folder [main](main).

ESP-IDF projects are built using CMake. The project build configuration is contained in `CMakeLists.txt`
files that provide set of directives and instructions describing the project's source files and targets
(executable, library, or both). 

Below is short explanation of remaining files in the project folder.

```
├── CMakeLists.txt
├── main
│   ├── CMakeLists.txt
│   └── main.c
└── README.md                  This is the file you are currently reading
```
Additionally, the sample project contains Makefile and component.mk files, used for the legacy Make based build system. 
They are not used or needed when building with CMake and idf.py.

## VL53L0X Example

This project demonstrates how to read distance measurements from a VL53L0X sensor using an ESP32-S3. The sensor is connected via the I2C bus using the new `esp_driver_i2c` master API. Default pins are GPIO18 (SDA) and GPIO19 (SCL). The `vl53l0x` component initializes the sensor and provides a helper function to retrieve the distance in millimetres.

Make sure the ESP-IDF tools are installed and the target is set to the ESP32-S3:

```bash
idf.py set-target esp32s3
idf.py menuconfig  # optional, generates sdkconfig
```

Then build and flash the project with:

```bash
idf.py build
idf.py flash
```

After reset, the application configures the VL53L0X sensor for back-to-back
continuous measurements and prints the measured distance to the serial console.
