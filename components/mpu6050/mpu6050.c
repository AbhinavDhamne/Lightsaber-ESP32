#include "mpu6050.h"
static const char *TAG = "MPU6050";

static float get_accel_sensitivity(mpu6050_accel_config_t);
static float get_gyro_sensitivity(mpu6050_gyro_config_t);

/**
 * @brief Reads a byte from the specified register of the MPU6050
 *
 * @param reg_addr Register address
 * @param data Pointer to store the read data
 * @return ESP_OK on success, ESP_FAIL on error
 */
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

/**
 * @brief Writes a byte to the specified register of the MPU6050
 *
 * @param reg_addr Register address
 * @param data Data to write
 * @return ESP_OK on success, ESP_FAIL on error
 */
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

/**
 * @brief Verifies the connection to the MPU6050
 *
 * @return ESP_OK on success, ESP_FAIL on error
 */
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



/**
 * @brief Initializes the MPU6050 with the given configuration
 *
 * @param config Pointer to the MPU6050 configuration structure
 * @return ESP_OK on success, ESP_FAIL on error
 */
esp_err_t mpu6050_init(const mpu6050_config_t *config)
{
    esp_err_t ret;

    ret = mpu6050_verify_connection();
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to verify MPU6050 connection: %s", esp_err_to_name(ret));
        return ret;
    }

    ret = mpu6050_write_register(MPU6050_RA_PWR_MGMT_1, 0x00); // Wake up the MPU6050
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Error waking up MPU6050: %s", esp_err_to_name(ret));
        return ret;
    }

    // Set sample rate divider
    ret = mpu6050_write_register(MPU6050_RA_SMPLRT_DIV, (uint8_t)config->sample_rate_divider);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Error setting MPU6050 sample rate divider: %s", esp_err_to_name(ret));
        return ret;
    }

    // Configure LPF
    ret = mpu6050_write_register(MPU6050_RA_CONFIG, (uint8_t)config->lpf_config);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Error configuring MPU6050 LPF: %s", esp_err_to_name(ret));
        return ret;
    }

    // Set gyro sensitivity
    ret = mpu6050_write_register(MPU6050_RA_GYRO_CONFIG, (uint8_t)config->gyro_config);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Error setting MPU6050 gyro sensitivity: %s", esp_err_to_name(ret));
        return ret;
    }

    // Set accelerometer sensitivity
    ret = mpu6050_write_register(MPU6050_RA_ACCEL_CONFIG, (uint8_t)config->accel_config);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Error setting MPU6050 accelerometer sensitivity: %s", esp_err_to_name(ret));
        return ret;
    }

    return ESP_OK;
}


/**
 * @brief Reads raw data from the MPU6050 and converts it to calibrated values
 *
 * @param data Pointer to the data structure to store the read data
 * @param config Pointer to the MPU6050 configuration structure
 * @return ESP_OK on success, ESP_FAIL on error
 */
esp_err_t mpu6050_read_data(mpu6050_data_t *data, const mpu6050_config_t *config)
{
    uint8_t reg_data_h, reg_data_l;
    esp_err_t ret;

    // Read accelerometer data
    ret = mpu6050_read_register(MPU6050_RA_ACCEL_XOUT_H, &reg_data_h);
    if (ret != ESP_OK)
        return ret;
    ret = mpu6050_read_register(MPU6050_RA_ACCEL_XOUT_L, &reg_data_l);
    if (ret != ESP_OK)
        return ret;
    data->acc_x_raw = (int16_t)((reg_data_h << 8) | reg_data_l);

    ret = mpu6050_read_register(MPU6050_RA_ACCEL_YOUT_H, &reg_data_h);
    if (ret != ESP_OK)
        return ret;
    ret = mpu6050_read_register(MPU6050_RA_ACCEL_YOUT_L, &reg_data_l);
    if (ret != ESP_OK)
        return ret;
    data->acc_y_raw = (int16_t)((reg_data_h << 8) | reg_data_l);

    ret = mpu6050_read_register(MPU6050_RA_ACCEL_ZOUT_H, &reg_data_h);
    if (ret != ESP_OK)
        return ret;
    ret = mpu6050_read_register(MPU6050_RA_ACCEL_ZOUT_L, &reg_data_l);
    if (ret != ESP_OK)
        return ret;
    data->acc_z_raw = (int16_t)((reg_data_h << 8) | reg_data_l);

    // Read temperature data
    ret = mpu6050_read_register(MPU6050_RA_TEMP_OUT_H, &reg_data_h);
    if (ret != ESP_OK)
        return ret;
    ret = mpu6050_read_register(MPU6050_RA_TEMP_OUT_L, &reg_data_l);
    if (ret != ESP_OK)
        return ret;
    data->temp_raw = (int16_t)((reg_data_h << 8) | reg_data_l);

    ret = mpu6050_read_register(MPU6050_RA_GYRO_XOUT_H, &reg_data_h);
    if (ret != ESP_OK)
        return ret;
    ret = mpu6050_read_register(MPU6050_RA_GYRO_XOUT_L, &reg_data_l);
    if (ret != ESP_OK)
        return ret;
    data->gyro_x_raw = (int16_t)((reg_data_h << 8) | reg_data_l);

    ret = mpu6050_read_register(MPU6050_RA_GYRO_YOUT_H, &reg_data_h);
    if (ret != ESP_OK)
        return ret;
    ret = mpu6050_read_register(MPU6050_RA_GYRO_YOUT_L, &reg_data_l);
    if (ret != ESP_OK)
        return ret;
    data->gyro_y_raw = (int16_t)((reg_data_h << 8) | reg_data_l);

    ret = mpu6050_read_register(MPU6050_RA_GYRO_ZOUT_H, &reg_data_h);
    if (ret != ESP_OK)
        return ret;
    ret = mpu6050_read_register(MPU6050_RA_GYRO_ZOUT_L, &reg_data_l);
    if (ret != ESP_OK)
        return ret;
    data->gyro_z_raw = (int16_t)((reg_data_h << 8) | reg_data_l);

    // Convert accelerometer data to g-force based on accel_config
    float accel_sensitivity = get_accel_sensitivity(config->accel_config);
    data->acc_x = (float)data->acc_x_raw / accel_sensitivity;
    data->acc_y = (float)data->acc_y_raw / accel_sensitivity;
    data->acc_z = (float)data->acc_z_raw / accel_sensitivity;

    // Convert gyroscope data to degrees per second based on gyro_config
    float gyro_sensitivity = get_gyro_sensitivity(config->gyro_config);
    data->gyro_x = (float)data->gyro_x_raw / gyro_sensitivity;
    data->gyro_y = (float)data->gyro_y_raw / gyro_sensitivity;
    data->gyro_z = (float)data->gyro_z_raw / gyro_sensitivity;

    // Convert temperature data to degrees Celsius
    data->temperature = (float)data->temp_raw / 340.0 + 36.53;

    return ESP_OK;
}


/**
 * @brief Calculates the accelerometer sensitivity based on the full scale range configuration
 *
 * @param config Accelerometer full scale range configuration
 * @return Accelerometer sensitivity factor
 */
static float get_accel_sensitivity(mpu6050_accel_config_t config)
{
    // Define sensitivity values based on full scale range
    const float sensitivity_factors[4] = {16384.0f, 8192.0f, 4096.0f, 2048.0f};
    return sensitivity_factors[config];
}




/**
 * @brief Calculates the gyroscope sensitivity based on the full scale range configuration
 *
 * @param config Gyroscope full scale range configuration
 * @return Gyroscope sensitivity factor
 */
static float get_gyro_sensitivity(mpu6050_gyro_config_t config)
{
    // Define sensitivity values based on full scale range
    const float sensitivity_factors[4] = {131.0f, 65.5f, 32.8f, 16.4f};
    return sensitivity_factors[config];
}