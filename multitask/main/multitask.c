#include <stdio.h>
#include <stdint.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"

/* Pines LEDs */
#define LED_HOG    GPIO_NUM_2
#define LED_OTHER  GPIO_NUM_4

/**
 * @brief Task principal
 */
static void TaskHog(void *pvParameters)
{
    volatile uint32_t counter = 0U;

    TickType_t xLastWakeTime;

    (void)pvParameters;

    xLastWakeTime = xTaskGetTickCount();

    while (1)
    {
        counter++;

        /* Cambia estado LED pin 2 */
        gpio_set_level(LED_HOG, counter % 2);

        if ((counter % 1000000U) == 0U)
        {
            printf(
                "[TaskHog] Running... counter=%lu\n",
                (unsigned long)counter
            );
        }

        /* ESP-IDF v6 usa xTaskDelayUntil */
        xTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(10));
    }
}

/**
 * @brief Segunda tarea
 */
static void TaskBlink(void *pvParameters)
{
    (void)pvParameters;

    while (1)
    {
        gpio_set_level(LED_OTHER, 1);
        vTaskDelay(pdMS_TO_TICKS(500));

        gpio_set_level(LED_OTHER, 0);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

/**
 * @brief Main
 */
void app_main(void)
{
    /* Configuración GPIO */
    gpio_reset_pin(LED_HOG);
    gpio_set_direction(LED_HOG, GPIO_MODE_OUTPUT);

    gpio_reset_pin(LED_OTHER);
    gpio_set_direction(LED_OTHER, GPIO_MODE_OUTPUT);

    printf("Multitask Example Started\n");

    /* Crear tareas */
    xTaskCreate(
        TaskHog,
        "TaskHog",
        2048,
        NULL,
        5,
        NULL);

    xTaskCreate(
        TaskBlink,
        "TaskBlink",
        2048,
        NULL,
        5,
        NULL);
}