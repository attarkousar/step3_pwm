#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"

#define POTENTIOMETER_PIN ADC1_CHANNEL_0   // ADC1 Channel 0 (GPIO 36) for potentiometer
#define LED_PIN           18               // GPIO pin for PWM output (LED)
#define ADC_MAX_VALUE     4095             // Max value for 12-bit ADC
#define PWM_FREQ          5000             // PWM frequency of 5kHz
#define PWM_RESOLUTION    LEDC_TIMER_10_BIT // 10-bit resolution for PWM
#define PWM_TIMER         LEDC_TIMER_0     // LEDC timer 0
#define PWM_CHANNEL       LEDC_CHANNEL_0   // LEDC channel 0

void configure_pwm() {
    // PWM Timer configuration
    ledc_timer_config_t ledc_timer = {
        .duty_resolution = PWM_RESOLUTION,
        .freq_hz = PWM_FREQ,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .timer_num = PWM_TIMER
    };
    ledc_timer_config(&ledc_timer);

    // PWM Channel configuration
    ledc_channel_config_t ledc_channel = {
        .channel    = PWM_CHANNEL,
        .duty       = 0,
        .gpio_num   = LED_PIN,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .hpoint     = 0,
        .timer_sel  = PWM_TIMER
    };
    ledc_channel_config(&ledc_channel);
}

void configure_adc() {
    adc1_config_width(ADC_WIDTH_BIT_12);               // Set ADC width to 12 bits
    adc1_config_channel_atten(POTENTIOMETER_PIN, ADC_ATTEN_DB_11);  // Set attenuation for max input voltage range
}

void read_potentiometer_task(void *pvParameter) {
    while (1) {
        int adc_value = adc1_get_raw(POTENTIOMETER_PIN); // Read raw ADC value from potentiometer
        int duty_cycle = (adc_value * ((1 << PWM_RESOLUTION) - 1)) / ADC_MAX_VALUE; // Map ADC to PWM duty cycle
        
        ledc_set_duty(LEDC_HIGH_SPEED_MODE, PWM_CHANNEL, duty_cycle);
        ledc_update_duty(LEDC_HIGH_SPEED_MODE, PWM_CHANNEL);
        
        printf("ADC Value: %d, Duty Cycle: %d\n", adc_value, duty_cycle);
        vTaskDelay(pdMS_TO_TICKS(100)); // Delay for stability
    }
}

// Optional feedback task to monitor and adjust LED brightness
void feedback_task(void *pvParameter) {
    while (1) {
        int actual_brightness = adc1_get_raw(POTENTIOMETER_PIN); // Placeholder for feedback ADC read
        // Insert logic here if needed for feedback adjustments
        vTaskDelay(pdMS_TO_TICKS(500)); // Delay to avoid frequent polling
    }
}

void app_main(void) {
    configure_pwm();
    configure_adc();
    xTaskCreate(read_potentiometer_task, "Read Potentiometer Task", 2048, NULL, 5, NULL);
    xTaskCreate(feedback_task, "Feedback Task", 2048, NULL, 5, NULL); // Optional feedback task
}
