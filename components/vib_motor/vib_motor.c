#include "vib_motor.h"

esp_err_t vib_motor_init(vib_motor_t *vib_motor)
{
    esp_err_t err;

    ledc_timer_config_t ledc_timer = {
        .speed_mode = vib_motor->speed_mode,
        .timer_num = vib_motor->timer_num,
        .duty_resolution = vib_motor->duty_resolution, // Use the value from vib_motor
        .freq_hz = vib_motor->freq_hz,                 // Use the value from vib_motor
        .clk_cfg = vib_motor->clk_cfg                  // Use the value from vib_motor
    };

    err = ledc_timer_config(&ledc_timer);

    if (err != ESP_OK)
    {
        return err; // Return error if channel configuration fails
    }

    ledc_channel_config_t ledc_channel = {
        .speed_mode = vib_motor->speed_mode,
        .channel = vib_motor->channel,
        .timer_sel = vib_motor->timer_num,
        .intr_type = vib_motor->intr_type, // Optional, use the value from vib_motor (if defined)
        .gpio_num = vib_motor->gpio_num,
        .duty = vib_motor->duty,
        .hpoint = vib_motor->hpoint};

    err = ledc_channel_config(&ledc_channel);

    if (err != ESP_OK)
    {
        return err; // Return error if channel configuration fails
    }

    return ESP_OK;
}
esp_err_t vib_motor_set_duty(vib_motor_t *motor, uint32_t new_duty)
{
    motor->duty = new_duty; // Update duty cycle in the vib_motor_t structure

    esp_err_t err = ledc_set_duty(motor->speed_mode, motor->channel, motor->duty);
    if (err != ESP_OK)
    {
        return err; // Return error if ledc_set_duty fails
    }

    err = ledc_update_duty(motor->speed_mode, motor->channel);
    if (err != ESP_OK)
    {
        return err; // Return error if ledc_update_duty fails
    }

    return ESP_OK;
}