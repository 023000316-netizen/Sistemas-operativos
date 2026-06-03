/**
 * @file race-condition-demo.c
 * @brief Race condition fixed using a mutex.
 */

#include <stdio.h>
#include <stdint.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#define TASK_ITERATIONS    (100000U)
#define TASK_DELAY_MS      (1U)

/* Shared resource */
static volatile uint32_t g_sharedCounter = 0U;

/* Mutex */
static SemaphoreHandle_t g_counterMutex = NULL;

/**
 * @brief Task that increments the shared counter.
 */
static void CounterTask(void *pvParameters)
{
    const char *taskName = (const char *)pvParameters;

    for (uint32_t i = 0U; i < TASK_ITERATIONS; i++)
    {
        if (xSemaphoreTake(g_counterMutex, portMAX_DELAY) == pdTRUE)
        {
            g_sharedCounter++;
            (void)xSemaphoreGive(g_counterMutex);
        }

        if ((i % 1000U) == 0U)
        {
            vTaskDelay(pdMS_TO_TICKS(TASK_DELAY_MS));
        }
    }

    printf("%s finished\n", taskName);

    vTaskDelete(NULL);
}

/**
 * @brief Main application entry point.
 */
void app_main(void)
{
    printf("\n");
    printf("=====================================\n");
    printf("   ESP32 Race Condition Demo Fixed\n");
    printf("=====================================\n");

    printf("Expected final value: %u\n",
           (TASK_ITERATIONS * 2U));

    g_counterMutex = xSemaphoreCreateMutex();

    if (g_counterMutex == NULL)
    {
        printf("Failed to create mutex\n");
        return;
    }

    printf("Mutex created successfully\n");

    xTaskCreate(
        CounterTask,
        "Task_A",
        2048U,
        (void *)"Task_A",
        5U,
        NULL);

    xTaskCreate(
        CounterTask,
        "Task_B",
        2048U,
        (void *)"Task_B",
        5U,
        NULL);

    /* Wait for both tasks to finish */
    vTaskDelay(pdMS_TO_TICKS(5000U));

    printf("Actual final value: %lu\n",
           (unsigned long)g_sharedCounter);

    vSemaphoreDelete(g_counterMutex);
}