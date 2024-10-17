#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"

#define PWM_GPIO        18              // GPIO pin for PWM output
#define PWM_FREQ        5000            // 5kHz frequency for PWM
#define PWM_RESOLUTION  LEDC_TIMER_10_BIT // 10-bit resolution
#define PWM_TIMER       LEDC_TIMER_0     // Use LEDC timer 0
#define PWM_CHANNEL     LEDC_CHANNEL_0   // Use LEDC channel 0

// Function to configure the PWM properties
void configure_pwm() {
    // Configure the PWM timer
    ledc_timer_config_t ledc_timer = {
        .duty_resolution = PWM_RESOLUTION,  // Resolution of PWM duty
        .freq_hz = PWM_FREQ,                // Frequency of PWM signal
        .speed_mode = LEDC_HIGH_SPEED_MODE, // Timer mode
        .timer_num = PWM_TIMER               // Timer index
    };
    ledc_timer_config(&ledc_timer);

    // Configure the PWM channel
    ledc_channel_config_t ledc_channel = {
        .channel    = PWM_CHANNEL,
        .duty       = 0,                    // Initial duty cycle
        .gpio_num   = PWM_GPIO,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .hpoint     = 0,
        .timer_sel  = PWM_TIMER
    };
    ledc_channel_config(&ledc_channel);
}

// Task to vary the PWM duty cycle
void pwm_task(void *pvParameter) {
    uint32_t duty = 0;

    while (1) {
        // Increase duty cycle
        for (duty = 0; duty <= (1 << PWM_RESOLUTION) - 1; duty++) {
            ledc_set_duty(LEDC_HIGH_SPEED_MODE, PWM_CHANNEL, duty);
            ledc_update_duty(LEDC_HIGH_SPEED_MODE, PWM_CHANNEL);
            vTaskDelay(pdMS_TO_TICKS(10)); // Delay for a smooth transition
        }
        
        // Decrease duty cycle
        for (duty = (1 << PWM_RESOLUTION) - 1; duty > 0; duty--) {
            ledc_set_duty(LEDC_HIGH_SPEED_MODE, PWM_CHANNEL, duty);
            ledc_update_duty(LEDC_HIGH_SPEED_MODE, PWM_CHANNEL);
            vTaskDelay(pdMS_TO_TICKS(10)); // Delay for a smooth transition
        }
    }
}

// Main application entry point
void app_main(void) {
    configure_pwm(); // Configure PWM settings
    xTaskCreate(pwm_task, "PWM Task", 2048, NULL, 5, NULL); // Create the PWM task
}
