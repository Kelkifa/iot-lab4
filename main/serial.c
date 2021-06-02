#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "sdkconfig.h"
#include "driver/gpio.h"

/**
 * This is an example which echos any data it receives on UART1 back to the sender,
 * with hardware flow control turned off. It does not use UART driver event queue.
 *
 * - Port: UART1
 * - Receive (Rx) buffer: on
 * - Transmit (Tx) buffer: off
 * - Flow control: off
 * - Event queue: off
 * - Pin assignment: see defines below
 */

#define BUF_SIZE (1024)
int index = 0;
static void echo_task()
{
    /* Configure parameters of an UART driver,
     * communication pins and install the driver */
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE};
    uart_driver_install(UART_NUM_0, BUF_SIZE * 2, 0, 0, NULL, 0);
    uart_param_config(UART_NUM_0, &uart_config);
    uart_set_pin(UART_NUM_0, 1, 3, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    // Configure a temporary buffer for the incoming data
    uint8_t *data = (uint8_t *)malloc(BUF_SIZE);

    while (1)
    {
        // Read data from the UART
        int len = uart_read_bytes(UART_NUM_0, data, BUF_SIZE, 1000 / portTICK_RATE_MS);
        // Write data back to the UART
        if (len > 0)
        {
            data[len] = 0;
            uart_write_bytes(UART_NUM_0, (const char *)data, len);
            printf("Data %d: %s\n", index, (const char *)data);
            index = index + 1;
            vTaskDelay(2000 / portTICK_PERIOD_MS);
        }
    }
}

void app_main()
{
    xTaskCreate(echo_task, "uart_echo_task", 1024 * 2, configMAX_PRIORITIES, 10, NULL);
}