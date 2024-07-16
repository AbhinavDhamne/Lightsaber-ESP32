#include "mpu6050.h"
static const char *TAG = "MPU6050";

esp_err_t mpu6050_read_register(uint8_t reg_addr, uint8_t *data)
{
    i2c_cmd_handle_t cmd_handle = i2c_cmd_link_create();
    esp_err_t ret;

    // Write register address to MPU6050
    i2c_master_start(cmd_handle);
    i2c_master_write_byte(cmd_handle, (MPU6050_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd_handle, reg_addr, true);
    i2c_master_stop(cmd_handle);
    ret = i2c_master_cmd_begin(I2C_NUM_0, cmd_handle, I2C_MASTER_TIMEOUT_MS);
    i2c_cmd_link_delete(cmd_handle);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Error writing to MPU6050: %s", esp_err_to_name(ret));
        i2c_cmd_link_delete(cmd_handle);
        return ret;
    }

    // Read data from register
    cmd_handle = i2c_cmd_link_create();
    i2c_master_start(cmd_handle);
    i2c_master_write_byte(cmd_handle, (MPU6050_ADDR << 1) | I2C_MASTER_READ, true);
    i2c_master_read_byte(cmd_handle, data, I2C_MASTER_NACK);
    i2c_master_stop(cmd_handle);
    ret = i2c_master_cmd_begin(I2C_NUM_0, cmd_handle, I2C_MASTER_TIMEOUT_MS);
    i2c_cmd_link_delete(cmd_handle);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Error reading from MPU6050: %s", esp_err_to_name(ret));
        return ret;
    }

    return ESP_OK;
}

esp_err_t mpu6050_write_register(uint8_t reg_addr, uint8_t data)
{
    i2c_cmd_handle_t cmd_handle = i2c_cmd_link_create();
    esp_err_t ret;

    // Write register address and data to MPU6050
    i2c_master_start(cmd_handle);
    i2c_master_write_byte(cmd_handle, (MPU6050_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd_handle, reg_addr, true);
    i2c_master_write_byte(cmd_handle, data, true);
    i2c_master_stop(cmd_handle);
    ret = i2c_master_cmd_begin(I2C_NUM_0, cmd_handle, I2C_MASTER_TIMEOUT_MS);
    i2c_cmd_link_delete(cmd_handle);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Error writing to MPU6050: %s", esp_err_to_name(ret));
        return ret;
    }

    return ESP_OK;
}

esp_err_t mpu6050_verify_connection()
{
    uint8_t who_am_i_data;
    esp_err_t ret;

    ret = mpu6050_read_register(WHO_AM_I_REG, &who_am_i_data);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Error reading WHO_AM_I register: %s", esp_err_to_name(ret));
        return ret;
    }

    if (who_am_i_data == MPU6050_ADDR)
    {
        ESP_LOGI(TAG, "MPU6050 found and properly responding.");
        return ESP_OK;
    }
    else
    {
        ESP_LOGE(TAG, "MPU6050 found, but response is unexpected (0x%02X)", who_am_i_data);
        return ESP_FAIL;
    }
}

  esp_err_t mpu6050_init(const mpu6050_config_t *config) {
    esp_err_t ret;

    ret = mpu6050_verify_connection();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to verify MPU6050 connection: %s", esp_err_to_name(ret));
        return ret;
    }

    ret = mpu6050_write_register(MPU6050_RA_PWR_MGMT_1, 0x00); // Wake up the MPU6050
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error waking up MPU6050: %s", esp_err_to_name(ret));
        return ret;
    }


    // Set sample rate divider
    ret = mpu6050_write_register(MPU6050_RA_SMPLRT_DIV, (uint8_t)config->sample_rate_divider);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error setting MPU6050 sample rate divider: %s", esp_err_to_name(ret));
        return ret;
    }

    // Configure LPF
    ret = mpu6050_write_register(MPU6050_RA_CONFIG, (uint8_t)config->lpf_config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error configuring MPU6050 LPF: %s", esp_err_to_name(ret));
        return ret;
    }

    // Set gyro sensitivity
    ret = mpu6050_write_register(MPU6050_RA_GYRO_CONFIG, (uint8_t)config->gyro_config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error setting MPU6050 gyro sensitivity: %s", esp_err_to_name(ret));
        return ret;
    }

    // Set accelerometer sensitivity
    ret = mpu6050_write_register(MPU6050_RA_ACCEL_CONFIG, (uint8_t)config->accel_config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error setting MPU6050 accelerometer sensitivity: %s", esp_err_to_name(ret));
        return ret;
    }

    return ESP_OK;
}
