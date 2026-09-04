/**
 * *****************************************************************************
 *  @file       Projects/HAL_examples/FreeRTOS/FreeRTOS_SMP/main.c
 *  @author     Baikal electronics SDK team
 *  @brief      HAL example source file
 *  @version    2.3.0
 *  @date       20260703
 * *****************************************************************************
 *  @copyright Copyright (c) 2026 Baikal Electronics JSC
 *
 *  This software is licensed under terms that can be found in the LICENSE file
 *  in the root directory of this software component.
 *  If no LICENSE file comes with this software, it is provided AS-IS.
 * *****************************************************************************
 */
#include <stdio.h>

#include "FreeRTOS.h" /* Must come first. */
#include "queue.h"
#include "semphr.h"
#include "task.h"

#include "bmcu_common.h"
#include "bsp.h"

#define mainQUEUE_RECEIVE_TASK_PRIORITY (tskIDLE_PRIORITY + 2)
#define mainQUEUE_SEND_TASK_PRIORITY (tskIDLE_PRIORITY + 1)
#define mainEVENT_SEMAPHORE_TASK_PRIORITY (configMAX_PRIORITIES - 1)

#define mainQUEUE_SEND_PERIOD_MS pdMS_TO_TICKS(200)

#define mainQUEUE_LENGTH (1)

/* Every 0.5 seconds */
#define mainLED_TOGGLE (500UL)

#define mainQUEUE_DATA_EXAMPLE (100UL)

static void prvSetupHardware(void);

static void prvQueueReceiveTask(void *pvParameters);
static void prvQueueSendTask(void *pvParameters);
static void prvEventSemaphoreTask(void *pvParameters);

static QueueHandle_t xQueue = NULL;
static SemaphoreHandle_t xEventSemaphore = NULL;

/* Statistics counters */
static volatile uint32_t ulCountOfItemsReceivedOnQueue = 0;
static volatile uint32_t ulCountOfItemsSentToQueue = 0;
static volatile uint32_t ulCountOfReceivedSemaphores = 0;

/*-----------------------------------------------------------*/

int
__io_putchar (int ch) {
    return bsp_serial_putchar(ch);
}

int
main (void) {
    prvSetupHardware();
    printf("\r\nFreeRTOS SMP demo example\r\n");
    xQueue = xQueueCreate(mainQUEUE_LENGTH, sizeof(uint32_t));

    /* **NOTE:** A semaphore is used for example purposes,
       using a direct to task notification will be faster
       and use less RAM. */
    xEventSemaphore = xSemaphoreCreateBinary();

    TaskHandle_t xHandle1, xHandle2, xHandle3;
    xTaskCreate(prvQueueReceiveTask,
                 "Rx",
                 configMINIMAL_STACK_SIZE,
                 NULL,
                 mainQUEUE_RECEIVE_TASK_PRIORITY,
                 &xHandle1);
    vTaskCoreAffinitySet(xHandle1, (1 << 1));
    xTaskCreate(prvQueueSendTask,
                 "TX",
                  configMINIMAL_STACK_SIZE,
                  NULL,
                  mainQUEUE_SEND_TASK_PRIORITY,
                  &xHandle2);
    vTaskCoreAffinitySet(xHandle2, (1 << 0));
    xTaskCreate(prvEventSemaphoreTask,
                 "Sem",
                 configMINIMAL_STACK_SIZE,
                 NULL,
                 mainEVENT_SEMAPHORE_TASK_PRIORITY,
                 &xHandle3);
    vTaskCoreAffinitySet(xHandle3, (1 << 1));
    vTaskStartScheduler();

    for(;;);
}

/*-----------------------------------------------------------*/

static void
prvQueueSendTask (void *pvParameters) {
    (void) pvParameters;
    const uint32_t ulValueToSend = mainQUEUE_DATA_EXAMPLE;

    for(;;) {
        xQueueSend(xQueue, &ulValueToSend, 0);
        __io_putchar('2');
        ulCountOfItemsSentToQueue++;
        vTaskDelay(mainQUEUE_SEND_PERIOD_MS);
    }
}

/*-----------------------------------------------------------*/

static void
prvQueueReceiveTask (void *pvParameters) {
    (void) pvParameters;
    uint32_t ulReceivedValue;

    for(;;) {
        xQueueReceive(xQueue, &ulReceivedValue, portMAX_DELAY);
        __io_putchar('1');

        if(ulReceivedValue == mainQUEUE_DATA_EXAMPLE) {
            ulCountOfItemsReceivedOnQueue++;
        }
    }
}

/*-----------------------------------------------------------*/

static void
prvEventSemaphoreTask (void *pvParameters) {
    (void) pvParameters;

    for(;;) {
        xSemaphoreTake(xEventSemaphore, portMAX_DELAY);

        /* Count the number of times the semaphore is received. */
        ulCountOfReceivedSemaphores++;
    }
}

/*-----------------------------------------------------------*/

void
vApplicationTickHook (void) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    static uint32_t ulCount = 0;

    ulCount++;
    if(ulCount >= mainLED_TOGGLE) {
        bsp_led_toggle();

        /* xHigherPriorityTaskWoken was initialised to pdFALSE, and will be set to
        pdTRUE by xSemaphoreGiveFromISR() if giving the semaphore unblocked a
        task that has equal or higher priority than the interrupted task. */
        xSemaphoreGiveFromISR(xEventSemaphore, &xHigherPriorityTaskWoken);
        ulCount = 0UL;
    }

    /* If xHigherPriorityTaskWoken is pdTRUE then a context switch should
    normally be performed before leaving the interrupt (because during the
    execution of the interrupt a task of equal or higher priority than the
    running task was unblocked).

    In this case, the function is running in the context of the tick interrupt,
    which will automatically check for the higher priority task to run anyway,
    so no further action is required. */
}

/*-----------------------------------------------------------*/

void
vApplicationStackOverflowHook (TaskHandle_t xTask, char *pcTaskName) {
    (void) pcTaskName;
    (void) xTask;

     for(;;);
}

/*-----------------------------------------------------------*/

void
vAssertCalled (void) {
    volatile uint32_t ulSetTo1ToExitFunction = 0;

    while(ulSetTo1ToExitFunction != 1) {
        __asm volatile ("NOP");
    }
}

/*-----------------------------------------------------------*/

static void
prvSetupHardware (void) {
    bsp_led_init();
    bsp_serial_init();

    CLIC_Config(1U, 1U);
    CLIC_SetLevelThreshold(0U);

    /* Configure timer interrupt */
    CLIC_ConfigIRQ(CLIC_MT_IRQn,                    /* Interrupt */
                   CLIC_INTATTR_MODE_MACHINE,       /* Privilege mode */
                   1U,                              /* Level */
                   1U,                              /* Priority */
                   CLIC_INTATTR_SHV_VECTORED,       /* Vector mode */
                   CLIC_INTATTR_TRIG_TYPE_LEVEL,    /* Type */
                   CLIC_INTATTR_TRIG_POL_P);        /* Polarity */
}
