/**
 * @file deadlock-demo.c
 * @brief Deadlock prevention example using FreeRTOS mutexes.
 */

#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

static SemaphoreHandle_t g_mutex1;
static SemaphoreHandle_t g_mutex2;

/**
 * @brief First task.
 */
static void TaskA(void *pvParameters)
{
    (void)pvParameters;

    while (1)
    {
        printf("TaskA: Taking Mutex1\n");

        xSemaphoreTake(g_mutex1, portMAX_DELAY);

        printf("TaskA: Mutex1 acquired\n");

        vTaskDelay(pdMS_TO_TICKS(1000U));

        printf("TaskA: Waiting for Mutex2\n");

        if (xSemaphoreTake(g_mutex2, pdMS_TO_TICKS(1000U)) == pdTRUE)
        {
            printf("TaskA: Mutex2 acquired\n");

            xSemaphoreGive(g_mutex2);
        }
        else
        {
            printf("TaskA: Timeout waiting for Mutex2\n");
        }

        xSemaphoreGive(g_mutex1);

        vTaskDelay(pdMS_TO_TICKS(100U));
    }
}

/**
 * @brief Second task.
 */
static void TaskB(void *pvParameters)
{
    (void)pvParameters;

    while (1)
    {
        printf("TaskB: Taking Mutex2\n");

        xSemaphoreTake(g_mutex2, portMAX_DELAY);

        printf("TaskB: Mutex2 acquired\n");

        vTaskDelay(pdMS_TO_TICKS(1000U));

        printf("TaskB: Waiting for Mutex1\n");

        if (xSemaphoreTake(g_mutex1, pdMS_TO_TICKS(1000U)) == pdTRUE)
        {
            printf("TaskB: Mutex1 acquired\n");

            xSemaphoreGive(g_mutex1);
        }
        else
        {
            printf("TaskB: Timeout waiting for Mutex1\n");
        }

        xSemaphoreGive(g_mutex2);

        vTaskDelay(pdMS_TO_TICKS(100U));
    }
}

void app_main(void)
{
    g_mutex1 = xSemaphoreCreateMutex();
    g_mutex2 = xSemaphoreCreateMutex();

    xTaskCreate(
        TaskA,
        "TaskA",
        4096U,
        NULL,
        5U,
        NULL);

    xTaskCreate(
        TaskB,
        "TaskB",
        4096U,
        NULL,
        5U,
        NULL);
}