/*****************************************************************/ /**
* @file uart_demo.h
* @brief
* @author bronson.zhan@quectel.com
* @date 2025-04-23
*
* @copyright Copyright (c) 2023 Quectel Wireless Solution, Co., Ltd.
* All Rights Reserved. Quectel Wireless Solution Proprietary and Confidential.
*
* @par EDIT HISTORY FOR MODULE
* <table>
* <tr><th>Date <th>Version <th>Author <th>Description"
* <tr><td>2025-04-23 <td>1.0 <td>Bronson.Zhan <td> Init
* </table>
**********************************************************************/
#ifndef __UART_DEMO_H__
#define __UART_DEMO_H__

#include "qosa_def.h"
#include "qosa_sys.h"

/*===========================================================================
 *  Macro Definition
 ===========================================================================*/

#define CONFIG_UNIRTOS_UART_DEMO_TASK_STACK_SIZE 4096
#define UNIR_UART_DEMO_TASK_PRIO                 QOSA_PRIORITY_NORMAL
#define UNIR_TEST_UART_PORT                      QOSA_UART_PORT_1
#define UNIR_TEST_UART_TX_PIN                    UNIR_UART1_TX_PIN
#define UNIR_TEST_UART_RX_PIN                    UNIR_UART1_RX_PIN
#define UNIR_TEST_UART_PIN_FUNC                  UNIR_UART1_PIN_FUNC
/* Pin definitions need to be configured according to the PINMUX table of the actual platform, as there are differences in definitions among different platforms
  The pin definitions need to be configured according to the PINMUX table of the actual platform.
  There are differences in the definitions for each platform.*/
#define UNIR_UART0_TX_PIN                        (39)
#define UNIR_UART0_RX_PIN                        (38)
#define UNIR_UART0_PIN_FUNC                      (1)

#define UNIR_UART1_TX_PIN                        (18)
#define UNIR_UART1_RX_PIN                        (17)
#define UNIR_UART1_PIN_FUNC                      (1)

#define UNIR_UART2_TX_PIN                        (29)
#define UNIR_UART2_RX_PIN                        (28)
#define UNIR_UART2_PIN_FUNC                      (3)

#define UNIR_UART3_TX_PIN                        (62)
#define UNIR_UART3_RX_PIN                        (63)
#define UNIR_UART3_PIN_FUNC                      (3)

/*===========================================================================
  *  Enum
  ===========================================================================*/

/**
  * @enum  qosa_uart_demo_case_e
  * @brief uart test case.
  */
typedef enum
{
    QOSA_UART_DEMO_OUTPUT = 0,       /*!< UART output test: continuous printing test */
    QOSA_UART_DEMO_READ_1,           /*!< UART read test 1: read as much as received */
    QOSA_UART_DEMO_READ_2,           /*!< UART read test 2: read 1024 bytes every 5 seconds, other caching (can be used to test flow control) */
    QOSA_UART_DEMO_BAUDRATE,         /*!< UART baud rate switching test: test switching between various baud rates */
    QOSA_UART_DEMO_CHANGE_CCIO_MODE, /*!< CCIO mode switching test: AT mode for 20s, user UART mode for 20s, print prompts during switching (this test is only for EIGEN platform, no effect on other platforms) */
    QOSA_UART_DEMO_MAX,
} qosa_uart_demo_case_e;

void unir_demo_uart_case_switch(qosa_uart_demo_case_e caseNo);
void unir_uart_demo_init(void);

#endif /* __UART_DEMO_H__ */
