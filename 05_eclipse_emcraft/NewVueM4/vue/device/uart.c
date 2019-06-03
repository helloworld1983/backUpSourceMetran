/******************************************************************************
 *
 *    File Name: uart.c
 *    @brief Copyright (c) 2018, Metran.  All Rights Reserved.
 *
 *
 *    Revision History:
 *
 *       Rev:      Date:       	Engineer:         Project:
 *        01       11/29/2018    Quyen Ngo         NewVue
 *        Description: New file
 ******************************************************************************/
#include "board/board.h"
#include "fsl_uart.h"
#include "uart.h"
#include "fsl_uart_freertos.h"
#include "fsl_debug_console.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define M4_UART_CLK_FREQ                BOARD_DEBUG_UART_CLK_FREQ
#define M4_UART_BAUDRATE                (115200U)

#define M4_UART_BUFFER                  (32)
#define M4_UART_PRIORITY                (configMAX_PRIORITIES - 2)

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
uart_rtos_handle_t handleUart1;
uart_rtos_handle_t handleUart2;
uart_rtos_handle_t handleUart3;
uart_rtos_handle_t handleUart4;

struct _uart_handle t_handle_uart1;
struct _uart_handle t_handle_uart2;
struct _uart_handle t_handle_uart3;
struct _uart_handle t_handle_uart4;

uint8_t background_buffer[M4_UART_BUFFER];
uint8_t recv_buffer[4] = {'\0'};
/*******************************************************************************
 * Code
 ******************************************************************************/
uart_rtos_config_t uart_config = {
    .baudrate = 115200,
    .parity = kUART_ParityDisabled,
    .stopbits = kUART_OneStopBit,
    .buffer = background_buffer,
    .buffer_size = sizeof(background_buffer),
};

void uart_Init(uint32_t channel, uint32_t baud)
{
    switch (channel) {
        case 3:
            uart_config.srcclk = BOARD_DEBUG_UART_CLK_FREQ;
            uart_config.base = UART3;
            uart_config.baudrate = baud;

            NVIC_SetPriority(UART3_IRQn, 3);

            if (0 > UART_RTOS_Init(&handleUart3, &t_handle_uart3, &uart_config))
            {
                vTaskSuspend(NULL);
            }
            break;
        default:
            break;
    }
}

int uart_Read(uint32_t channel, uint8_t* buf, uint32_t len)
{
    int stat = kStatus_Fail;
    size_t n;

    switch (channel) {
        case 1:
            stat = UART_RTOS_Receive(&handleUart1, buf, len, &n);
            break;
        case 2:
            stat = UART_RTOS_Receive(&handleUart2, buf, len, &n);
            break;
        case 3:
            stat = UART_RTOS_Receive(&handleUart3, buf, len, &n);
            break;
        case 4:
            stat = UART_RTOS_Receive(&handleUart4, buf, len, &n);
            break;
        default:
            break;
    }

    if (stat == kStatus_UART_RxHardwareOverrun){
        /* Notify about hardware buffer overrun */
        PRINTF("Hardware buffer overrun! \n");
    }
    if (stat == kStatus_UART_RxRingBufferOverrun){
        /* Notify about ring buffer overrun */
        PRINTF("Ring buffer overrun! \n");
    }


    if(kStatus_Success == stat){
        return 0;
    }else{
        PRINTF("Err Uart catch up in channel %d and ignored %d \n", channel, stat);
        return stat;
    }
}

void uart_Write(uint32_t channel, uint8_t* buf, uint32_t len)
{
    switch (channel) {
        case 1:
            if (kStatus_Success != UART_RTOS_Send(&handleUart1, (uint8_t *)buf, len)){
                vTaskSuspend(NULL);
            }
            break;
        case 2:
            if (kStatus_Success != UART_RTOS_Send(&handleUart2, (uint8_t *)buf, len)){
                vTaskSuspend(NULL);
            }
            break;
        case 3:
            if (kStatus_Success != UART_RTOS_Send(&handleUart3, (uint8_t *)buf, len)){
                vTaskSuspend(NULL);
            }
            break;
        case 4:
            if (kStatus_Success != UART_RTOS_Send(&handleUart4, (uint8_t *)buf, len)){
                vTaskSuspend(NULL);
            }
            break;
        default:
            break;
    }
}

void uart_Deinit(uint32_t channel)
{
    switch (channel) {
        case 1:
            UART_RTOS_Deinit(&handleUart1);
            break;
        case 2:
            UART_RTOS_Deinit(&handleUart2);
            break;
        case 3:
            UART_RTOS_Deinit(&handleUart3);
            break;
        case 4:
            UART_RTOS_Deinit(&handleUart4);
            break;
        default:
            break;
    }
}
