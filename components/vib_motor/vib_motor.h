#ifndef VIB_MOTO_H
#define VIB_MOTOR_H



#include <esp_err.h>
#include "driver/ledc.h"

typedef struct {
    // Timer configuration (from ledc_timer_config_t)
    ledc_mode_t speed_mode;
    ledc_timer_bit_t duty_resolution;
    ledc_timer_t timer_num;
    uint32_t freq_hz;
    ledc_clk_cfg_t clk_cfg;

    // Channel configuration (from ledc_channel_config_t)
    int gpio_num;
    ledc_channel_t channel;
    ledc_intr_type_t intr_type;  // Optional, can be removed if not needed

    // Duty cycle and hpoint (from ledc_channel_config_t)
    uint32_t duty;
    int hpoint;

    // Output invert flag (from ledc_channel_config_t.flags)
    bool output_invert;  // Using a more descriptive type (bool)

} vib_motor_t;

esp_err_t vib_motor_init(vib_motor_t* vib_motor) ;
esp_err_t vib_motor_set_duty(vib_motor_t *motor, uint32_t new_duty);

#endif