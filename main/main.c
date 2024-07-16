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

//static const char *TAG = "MPU6050";
/*
esp_err_t verifyMPU6050Connection() {
    i2c_cmd_handle_t cmd_handle = i2c_cmd_link_create();
    esp_err_t ret;

    // Write register address to MPU6050 (check WHO_AM_I register)
    i2c_master_start(cmd_handle);
    i2c_master_write_byte(cmd_handle, (MPU6050_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd_handle, WHO_AM_I_REG, true);

    i2c_master_stop(cmd_handle);
    ret = i2c_master_cmd_begin(I2C_NUM_0, cmd_handle, I2C_MASTER_TIMEOUT_MS);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error writing to MPU6050: %s", esp_err_to_name(ret));
        i2c_cmd_link_delete(cmd_handle);
        return ret;
    }

    // Read data from WHO_AM_I register
    uint8_t reg_data;
   i2c_master_start(cmd_handle);
    i2c_master_write_byte(cmd_handle, (MPU6050_ADDR << 1) | I2C_MASTER_READ, true);
    i2c_master_read_byte(cmd_handle, &reg_data, I2C_MASTER_NACK);
    i2c_master_stop(cmd_handle);
    ret = i2c_master_cmd_begin(I2C_NUM_0, cmd_handle, I2C_MASTER_TIMEOUT_MS);
    i2c_cmd_link_delete(cmd_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error reading from MPU6050: %s", esp_err_to_name(ret));
        return ret;
    }

    if (reg_data == 0x68) {
        ESP_LOGI(TAG, "MPU6050 found and properly responding.");
        return ESP_OK;
    } else {
        ESP_LOGE(TAG, "MPU6050 found, but response is unexpected (0x%02X)", reg_data);
        return ESP_FAIL;
    }
}



esp_err_t readMPU6050Data(uint8_t *data)
{
    i2c_cmd_handle_t cmd_handle = i2c_cmd_link_create();
    esp_err_t ret;

    // Write register address to MPU6050
    i2c_master_start(cmd_handle);
    i2c_master_write_byte(cmd_handle, (MPU6050_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd_handle, WHO_AM_I_REG, true);
    i2c_master_stop(cmd_handle);
    ret = i2c_master_cmd_begin(I2C_NUM_0, cmd_handle, I2C_MASTER_TIMEOUT_MS);
    i2c_cmd_link_delete(cmd_handle);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Error writing to MPU6050: %s", esp_err_to_name(ret));
        i2c_cmd_link_delete(cmd_handle);
        return ret;
    }

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    cmd = i2c_cmd_link_create();

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (MPU6050_ADDR << 1) | I2C_MASTER_READ, true);
    i2c_master_read_byte(cmd, data, I2C_MASTER_NACK);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, I2C_MASTER_TIMEOUT_MS);
    i2c_cmd_link_delete(cmd);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Error reading from MPU6050: %s", esp_err_to_name(ret));
        return ret;
    }

    return ESP_OK;
}
*/

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
        .accel_config = MPU6050_ACCEL_FS_16G
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
}
/*

void app_main(void){

  i2c_config_t i2c_config = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = SDA_GPIO,
        .scl_io_num = SCL_GPIO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 400000 };
    i2c_param_config(I2C_NUM_0, &i2c_config);
    i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0);

    while(1){

    esp_err_t ret = verifyMPU6050Connection();
    if (ret != ESP_OK) {
        ESP_LOGE("main", "Failed to verify MPU6050 connection: %s", esp_err_to_name(ret));
        // Handle error, e.g., retry or indicate failure
    }
vTaskDelay(pdMS_TO_TICKS(1000));
    }



}

*/