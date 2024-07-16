// Libraries
#include <driver/gpio.h>
#include <driver/i2c.h>
#include <esp_err.h>
#include <esp_log.h>
#include <esp_sleep.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <math.h>
#include <stdio.h>

#include "mpu6050.h"

#define SDA_GPIO 21
#define SCL_GPIO 22



void app_main(void)
{
    i2c_config_t i2c_config = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = SDA_GPIO,
        .scl_io_num = SCL_GPIO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 400000};
    i2c_param_config(I2C_NUM_0, &i2c_config);
    i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0);

   mpu6050_config_t config = {
        .sample_rate_divider = MPU6050_SAMPLE_RATE_DIV_1KHZ,  // Example configuration
        .lpf_config = MPU6050_LPF_CONFIG_42HZ,
        .gyro_config = MPU6050_GYRO_FS_2000DPS,
        .accel_config = MPU6050_ACCEL_FS_2G
    };

    // Initialize the MPU6050
    esp_err_t ret = mpu6050_init(&config);
    if (ret != ESP_OK) {
        ESP_LOGE("main", "Failed to initialize MPU6050: %s", esp_err_to_name(ret));
        // Handle error
    } else {
        ESP_LOGI("main", "MPU6050 initialized successfully");
        // Continue with MPU6050 operations
        
    }


    mpu6050_data_t data;

    while (1) {
        mpu6050_read_data(&data, &config);

        printf("Raw Accel: X=%d, Y=%d, Z=%d\n", data.acc_x_raw, data.acc_y_raw, data.acc_z_raw);
        printf("Accel: X=%.2f g, Y=%.2f g, Z=%.2f g\n", data.acc_x, data.acc_y, data.acc_z);

        // ... print other data as needed

        vTaskDelay(1000 / portTICK_PERIOD_MS); // Delay for 1 second
    }
}
