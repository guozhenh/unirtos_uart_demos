/*****************************************************************/ /**
* @file uart_demo.c
* @brief
* @author bronson.zhan@quectel.com
* @date 2025-04-23
*
* @copyright Copyright (c) 2023 Quectel Wireless Solution, Co., Ltd.
* All Rights Reserved. Quectel Wireless Solution Proprietary and Confidential.
*
* @par EDIT HISTORY FOR MODULE
* <table>
* <tr><th>Date <th>Version <th>Author <th>Description
* <tr><td>2025-04-23 <td>1.0 <td>Bronson.Zhan <td> Init
* </table>
**********************************************************************/
#include "qosa_sys.h"
#include "qosa_uart.h"
#include "qosa_def.h"
#include "qosa_log.h"
#include "qosa_gpio.h"
#include "uart_demo.h"

/*===========================================================================
 *  Macro Definition
 ===========================================================================*/

#define QOS_LOG_TAG LOG_TAG_UART_API

/*===========================================================================
 *  Variate
 ===========================================================================*/

static qosa_task_t  g_unir_uart_demo_task = QOSA_NULL;
static qosa_uint8_t g_uart_data[1024] = {0};

static qosa_uint16_t g_uart_test_case = QOSA_UART_DEMO_OUTPUT;

/*===========================================================================
 *  Static API Functions
 ===========================================================================*/
/**
 * @brief UART event callback processing function
 *
 * This function is used to handle various event indications of UART ports, including receive data, transmit completion, and transmit buffer low water level events.
 * When an event occurs, the event information will be sent out via UART.
 *
 * @param cb_param UART callback parameter structure pointer, contains port number, event ID, user data and other information
 */
static void unir_uart_ind(qosa_uart_cb_param_t *cb_param)
{
    qosa_uart_port_number_e port = cb_param->port;
    qosa_uint32_t           event_id = cb_param->event_id;
    char                    data[128] = {0};
    qosa_snprintf(data, sizeof(data), "port=%d, event_id=%d, user_data=%s", port, event_id, (unsigned char *)cb_param->user_data);
    // Process accordingly based on different UART event types
    if (cb_param->event_id & QOSA_UART_EVENT_RX_INDICATE)
    {
        qosa_uart_write(port, (unsigned char *)data, sizeof(data));
    }
    else if (cb_param->event_id & QOSA_UART_EVENT_TX_COMPLETE)
    {
        qosa_uart_write(port, (unsigned char *)data, sizeof(data));
    }
    else if (cb_param->event_id & QOSA_UART_EVENT_TX_LOW)
    {
        qosa_uart_write(port, (unsigned char *)data, sizeof(data));
    }
}

/**
 * @brief UART function processing function, used to configure and test various functions of UART interface.
 *
 * This function initializes UART port, registers callback functions, and executes different test cases based on the value of global variable g_uart_test_case,
 * including output data, read data, baud rate switching, and mode switching operations.
 *
 */
static void unir_uart_demo_process(void *ctx)
{
    int ret = 0;

    qosa_uart_status_monitor_t monitor = {0};
    monitor.callback = unir_uart_ind; /* Register callback function */
    monitor.event_mask = QOSA_UART_EVENT_RX_INDICATE | QOSA_UART_EVENT_TX_COMPLETE;
    monitor.user_data = "Hello, Uart!";

    /* Register UART event callback */
    qosa_uart_register_cb(UNIR_TEST_UART_PORT, &monitor);

    /* Configure UART communication parameters: baud rate, data bits, stop bits, parity bit, flow control */
    qosa_uart_config_t dcb_config = {0};
    dcb_config.baudrate = QOSA_UART_BAUD_115200;
    dcb_config.data_bit = QOSA_UART_DATABIT_8;
    dcb_config.flow_ctrl = QOSA_FC_NONE;
    dcb_config.parity_bit = QOSA_UART_PARITY_NONE;
    dcb_config.stop_bit = QOSA_UART_STOP_1;

    qosa_uart_ioctl(UNIR_TEST_UART_PORT, QOSA_UART_IOCTL_SET_DCB_CFG, (void *)&dcb_config);
    /* Configure UART port pin functions */
    qosa_pin_set_func(UNIR_TEST_UART_TX_PIN,UNIR_TEST_UART_PIN_FUNC);
    qosa_pin_set_func(UNIR_TEST_UART_RX_PIN,UNIR_TEST_UART_PIN_FUNC);
    /* Open UART port */
    qosa_uart_open(UNIR_TEST_UART_PORT);

    while (1)
    {
        switch (g_uart_test_case)
        {
            /* Test UART transmit function */
            case QOSA_UART_DEMO_OUTPUT: {
                qosa_task_sleep_sec(1);
                qosa_uart_write(UNIR_TEST_UART_PORT, (unsigned char *)"hello UniRTOS\r\n", 15);
            }
            break;
            /* Test UART receive function (processed via callback) */
            case QOSA_UART_DEMO_READ_1: {
                qosa_task_sleep_sec(1);
                /* Received data in uart callback */
            }
            break;
            /* Test UART receive function (active reading) */
            case QOSA_UART_DEMO_READ_2: {
                qosa_task_sleep_sec(5);
                qosa_uart_read(UNIR_TEST_UART_PORT, (unsigned char *)&g_uart_data, 1024);

                QLOGI("recv uart data %s", g_uart_data);
                ret = qosa_uart_write(UNIR_TEST_UART_PORT, (unsigned char *)&g_uart_data, 1024);
                QLOGI("qosa_uart_write ret = %d", ret);
            }
            break;
            /* Test UART communication at different baud rates */
            case QOSA_UART_DEMO_BAUDRATE: {
                const qosa_uint32_t baudRateList[] = {0, 600, 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200, 230400, 460800, 921600};

                int i;
                for (i = 0; i < sizeof(baudRateList) / sizeof(baudRateList[0]); i++)
                {
                    qosa_uart_ioctl(UNIR_TEST_UART_PORT, QOSA_UART_IOCTL_CHANGE_BAUDRATE, (void *)&baudRateList[i]);
                    qosa_task_sleep_sec(1);
                    qosa_uart_write(UNIR_TEST_UART_PORT, (unsigned char *)"Baudrate TEST\r\n", 15);
                    qosa_task_sleep_sec(1);
                }
            }
            break;
            /* Test UART mode switching function (Uart mode and AT command mode) */
            case QOSA_UART_DEMO_CHANGE_CCIO_MODE: {
                qosa_uart_mode_e ccio_mode;
                int              i;
                ccio_mode = QOSA_UART_MODE_NORMAL;
                qosa_uart_ioctl(UNIR_TEST_UART_PORT, QOSA_UART_IOCTL_SET_CCIO_MODE, (void *)&ccio_mode);
                qosa_uart_write(UNIR_TEST_UART_PORT, (unsigned char *)"Enter Uart Mode\r\n", 17);
                for (i = 0; i < 20; i++)
                {
                    qosa_uart_write(UNIR_TEST_UART_PORT, (unsigned char *)"Waiting...\r\n", 12);
                    qosa_task_sleep_sec(1);
                }

                ccio_mode = QOSA_UART_MODE_AT;
                qosa_uart_write(UNIR_TEST_UART_PORT, (unsigned char *)"Enter AT Mode\r\n", 15);
                qosa_task_sleep_sec(1); /* Wait for transmission to complete */
                qosa_uart_ioctl(UNIR_TEST_UART_PORT, QOSA_UART_IOCTL_SET_CCIO_MODE, (void *)&ccio_mode);
                qosa_task_sleep_sec(20);
            }
            break;
            default:
                break;
        }
    }
}

/*===========================================================================
 *  Public API Functions
 ===========================================================================*/

void unir_demo_uart_case_switch(qosa_uart_demo_case_e caseNo)
{
    g_uart_test_case = caseNo;
}

void unir_uart_demo_init(void)
{
    QLOGI("enter UniRTOS UART DEMO !!!");
    if (g_unir_uart_demo_task == QOSA_NULL)
    {
        qosa_task_create(
            &g_unir_uart_demo_task,
            CONFIG_UNIRTOS_UART_DEMO_TASK_STACK_SIZE,
            UNIR_UART_DEMO_TASK_PRIO,
            "uart_demo",
            unir_uart_demo_process,
            QOSA_NULL,
            1
        );
    }
}
