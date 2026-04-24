#include "cli_commands.h"
#include "FreeRTOS.h"
#include "FreeRTOS_CLI.h"
#include "board.h"
#include "fsl_gpio.h"      // Required for GPIO_PinWrite
#include "fsl_usart.h"     // Required for USART_WriteBlocking
#include "stream_buffer.h" // Required for xStreamBufferReceive
#include "task.h"
#include <stdio.h>
#include <string.h>

#define WAIT_TIME 2000

/* Tell the compiler this lives in another file */
extern volatile bool g_uartResponseMapping;
extern StreamBufferHandle_t xUart1RxStream;
/* Extern the handle created in your main/hardware init */
extern StreamBufferHandle_t xUart1RxStream;

/* RGB command definition */
static BaseType_t prvRGBCommand(char *pcWriteBuffer, size_t xWriteBufferLen,
                                const char *pcCommandString);

/* UART Query command definition */
static BaseType_t prvUARTCommand(char *pcWriteBuffer, size_t xWriteBufferLen,
                                 const char *pcCommandString);

static const CLI_Command_Definition_t xRGBCommand = {
    "rgb",
    "\r\nrgb <color>:\r\n Set RGB LED color (e.g. 'rgb red', 'rgb blue')\r\n",
    prvRGBCommand, 1};

static const CLI_Command_Definition_t xUARTQueryCommand = {
    "query",
    "\r\nquery <str>:\r\n Sends <str> to UART1 and waits 5000ms for a "
    "response\r\n",
    prvUARTCommand, 1};

/* --- RGB Command Handler --- */
static BaseType_t prvRGBCommand(char *pcWriteBuffer, size_t xWriteBufferLen,
                                const char *pcCommandString) {
  const char *pcParameter;
  BaseType_t xParameterStringLength;

  pcParameter =
      FreeRTOS_CLIGetParameter(pcCommandString, 1, &xParameterStringLength);

  if (pcParameter != NULL) {
    if (strncmp(pcParameter, "red", xParameterStringLength) == 0 &&
        xParameterStringLength == 3) {
      LED_RED_ON();
      LED_BLUE_OFF();
      LED_GREEN_OFF();
      strncpy(pcWriteBuffer, "Red LED ON\r\n", xWriteBufferLen);
    } else if (strncmp(pcParameter, "blue", xParameterStringLength) == 0 &&
               xParameterStringLength == 4) {
      LED_RED_OFF();
      LED_BLUE_ON();
      LED_GREEN_OFF();
      strncpy(pcWriteBuffer, "Blue LED ON\r\n", xWriteBufferLen);
    } else if (strncmp(pcParameter, "green", xParameterStringLength) == 0 &&
               xParameterStringLength == 5) {
      LED_RED_OFF();
      LED_BLUE_OFF();
      LED_GREEN_ON();
      strncpy(pcWriteBuffer, "Green LED ON\r\n", xWriteBufferLen);
    } else if (strncmp(pcParameter, "off", xParameterStringLength) == 0 &&
               xParameterStringLength == 3) {
      LED_RED_OFF();
      LED_BLUE_OFF();
      LED_GREEN_OFF();
      strncpy(pcWriteBuffer, "LEDs OFF\r\n", xWriteBufferLen);
    } else {
      strncpy(pcWriteBuffer, "Unknown color. Red/Green/Blue/Off supported.\r\n",
              xWriteBufferLen);
    }
  } else {
    strncpy(pcWriteBuffer, "Missing parameter.\r\n", xWriteBufferLen);
  }
  return pdFALSE;
}

extern uint32_t SystemCoreClock;

/* --- Software Bit-Banged UART TX (9600 Baud) --- */
static void prvBitBangUartTx(uint8_t data) {
  uint32_t cycles_per_bit = SystemCoreClock / 9600;

  /* Enable DWT Cycle Counter for absolute precision */
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

  /* Enter critical section to prevent RTOS ticks from messing up our bit timing
   */
  taskENTER_CRITICAL();

  uint32_t start_cycle = DWT->CYCCNT;
  uint32_t target = start_cycle;

  /* Start bit (LOW) */
  GPIO_PinWrite(GPIO, 0U, 14U, 0U);
  target += cycles_per_bit;
  while ((DWT->CYCCNT - start_cycle) < (target - start_cycle)) {
  }

  /* 8 Data bits (LSB first) */
  for (int i = 0; i < 8; i++) {
    uint8_t bit = (data >> i) & 0x01;
    GPIO_PinWrite(GPIO, 0U, 14U, bit);
    target += cycles_per_bit;
    while ((DWT->CYCCNT - start_cycle) < (target - start_cycle)) {
    }
  }

  /* Stop bit (HIGH) */
  GPIO_PinWrite(GPIO, 0U, 14U, 1U);
  target += cycles_per_bit;
  while ((DWT->CYCCNT - start_cycle) < (target - start_cycle)) {
  }

  taskEXIT_CRITICAL();
}

/* --- UART Query Command Handler --- */
static BaseType_t prvUARTCommand(char *pcWriteBuffer, size_t xWriteBufferLen,
                                 const char *pcCommandString) {
  const char *pcParameter;
  BaseType_t xParameterStringLength;
  uint8_t ucRxByte;
  int offset = 0;

  /* 1. Extract the string to send */
  pcParameter =
      FreeRTOS_CLIGetParameter(pcCommandString, 1, &xParameterStringLength);

  if (pcParameter == NULL) {
    strncpy(pcWriteBuffer, "Usage: query <string>\r\n", xWriteBufferLen);
    return pdFALSE;
  }

  /* Log start of transmission */
  offset += snprintf(
      pcWriteBuffer + offset, xWriteBufferLen - offset,
      "\r\n[DIAG] Sending '%s' (%d bytes)...\r\n[DIAG] RX Data: ", pcParameter,
      (int)xParameterStringLength);

  /* 2. Prepare for Response: Route incoming data to our stream buffer */
  g_uartResponseMapping = true;

  /* Set DE High (Transmit Mode) */
  GPIO_PinWrite(GPIO, 0U, 20U, 1U);

  /* 3. Send the command + single newline to target */
  /* --- Software Bit-Banged UART Logic --- */
  /* Re-mux PIO0_14 to GPIO (FUNC0) */
  uint32_t port0_pin14_gpio =
      (0x00u | 0x0100u | (0x1 << 5)); /* FUNC0 | DIGITAL_EN | GPIO_MODE */
  IOCON->PIO[0][14] = port0_pin14_gpio;

  /* Set PIO0_14 direction to output and drive HIGH (Idle state) */
  GPIO->DIR[0] |= (1U << 14U);
  GPIO_PinWrite(GPIO, 0U, 14U, 1U);

  /* Allow the line to idle high for a moment before sending */
  vTaskDelay(pdMS_TO_TICKS(2));

  /* Bit-bang the command string */
  for (size_t i = 0; i < xParameterStringLength; i++) {
    prvBitBangUartTx((uint8_t)pcParameter[i]);
  }
  /* Bit-bang the newline */
  prvBitBangUartTx((uint8_t)'\n');

  /* Re-mux PIO0_14 back to FC1_TXD (FUNC1) */
  uint32_t port0_pin14_uart =
      (0x01u | 0x0100u | (0x1 << 5)); /* FUNC1 | DIGITAL_EN | GPIO_MODE */
  IOCON->PIO[0][14] = port0_pin14_uart;
  /* ----------------------------------- */

  /* Set DE Low (Receive Mode) */
  GPIO_PinWrite(GPIO, 0U, 20U, 0U);

  /* --- Software Bit-Banged UART RX Logic --- */
  /* 1. Re-mux PIO0_13 to GPIO Input */
  uint32_t port0_pin13_gpio =
      (0x00u | 0x0100u | (0x1 << 5)); /* FUNC0 | DIGITAL_EN | GPIO_MODE */
  IOCON->PIO[0][13] = port0_pin13_gpio;
  GPIO->DIR[0] &= ~(1U << 13U); /* Set to input */

  /* 2. Wait exactly 1ms to yield CPU, ensuring we wake up before the 5ms ESP32
   * turnaround */
  vTaskDelay(pdMS_TO_TICKS(1));

  /* 3. Elevate Task Priority to prevent RTOS starvation while polling */
  UBaseType_t uxSavedPriority = uxTaskPriorityGet(NULL);
  vTaskPrioritySet(NULL, configMAX_PRIORITIES - 1);

  /* Enable DWT Cycle Counter for absolute precision */
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

  uint8_t raw_rx_buf[64];
  int rx_index = 0;
  uint32_t timeout_us = 10000; /* 10ms timeout for the first byte to safely
                                  catch the 5ms ESP32 delay */

  /* A conservative estimate of cycles per spin loop to calculate timeout */
  const uint32_t cycles_per_spin = 10;
  uint32_t spins_per_us = (SystemCoreClock / 1000000) / cycles_per_spin;
  uint32_t cycles_per_bit = SystemCoreClock / 9600;

  while (rx_index < sizeof(raw_rx_buf)) {
    uint32_t max_spins = timeout_us * spins_per_us;
    bool timeout = true;

    /* Poll for falling edge (Start Bit) */
    while (max_spins-- > 0) {
      if (GPIO_PinRead(GPIO, 0U, 13U) == 0) {
        timeout = false;
        break;
      }
    }

    if (timeout) {
      break; /* Message finished or gap */
    }

    /* Start bit detected! We are now at the FALLING EDGE. */
    uint32_t start_cycle = DWT->CYCCNT;
    uint32_t target = start_cycle;

    /* Wait 1.5 bit times to reach center of D0 */
    target += cycles_per_bit + (cycles_per_bit / 2);
    while ((DWT->CYCCNT - start_cycle) < (target - start_cycle)) {
    }

    uint8_t byte = 0;
    for (int i = 0; i < 8; i++) {
      if (GPIO_PinRead(GPIO, 0U, 13U)) {
        byte |= (1 << i);
      }
      target += cycles_per_bit;
      while ((DWT->CYCCNT - start_cycle) < (target - start_cycle)) {
      }
    }
    /* The for-loop finishes exactly at 9.5 bit times (the center of the stop
       bit). We DO NOT need to delay any further! If we delay another bit time,
       we will oversleep the 10.0 start bit of the next byte! We can immediately
       safely loop back to poll for the next falling edge. */

    raw_rx_buf[rx_index++] = byte;

    /* Keep collecting even after newline to grab trailing \r or garbage,
       but reduce timeout */
    timeout_us = 2000; /* 2ms timeout for subsequent bytes */
  }

  /* 4. Restore RTOS Task Priority */
  vTaskPrioritySet(NULL, uxSavedPriority);

  /* 5. Restore PIO0_13 to FC1_RXD */
  uint32_t port0_pin13_uart =
      (0x01u | 0x0100u | (0x1 << 5)); /* FUNC1 | DIGITAL_EN | GPIO_MODE */
  IOCON->PIO[0][13] = port0_pin13_uart;
  /* ----------------------------------------- */

  /* 5. flip the switch back. */
  g_uartResponseMapping = false;

  /* 6. Format the final output from the raw buffer */
  for (int i = 0; i < rx_index; i++) {
    uint8_t b = raw_rx_buf[i];
    if (b >= 32 && b <= 126) {
      offset +=
          snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset, "%c", b);
    } else {
      offset += snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset,
                         "\\x%02X", b);
    }
  }

  if (rx_index == 0) {
    snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset,
             "\r\n[DIAG] Error: UART Timeout (0 bytes RX)\r\n");
  } else {
    snprintf(pcWriteBuffer + offset, xWriteBufferLen - offset,
             "\r\n[DIAG] Success: %d bytes RX\r\n", rx_index);
  }

  return pdFALSE;
}

///* --- UART Query Command Handler --- */
// static BaseType_t prvUARTCommand(char *pcWriteBuffer, size_t xWriteBufferLen,
//                                 const char *pcCommandString) {
//     const char *pcParameter;
//     BaseType_t xParameterStringLength;
//     size_t xReceivedBytes;
//
//     /* 1. Extract the string to send */
//     pcParameter = FreeRTOS_CLIGetParameter(pcCommandString, 1,
//     &xParameterStringLength);
//
//     if (pcParameter == NULL) {
//         strncpy(pcWriteBuffer, "Usage: query <string>\r\n", xWriteBufferLen);
//         return pdFALSE;
//     }
//
//     /* 2. Flush any old data sitting in the stream buffer */
//     xStreamBufferReset(xUart0RxStream);
//
//     /* 3. Send the string + a newline to UART0 */
//     USART_WriteBlocking(USART0, (uint8_t *)pcParameter,
//     xParameterStringLength); USART_WriteBlocking(USART0, (uint8_t *)"\r\n",
//     2);
//
//     /* 4. Block and wait for a response.
//           pcWriteBuffer is used as the temporary storage for the RX data. */
//     xReceivedBytes = xStreamBufferReceive(xUart0RxStream,
//                                           (void *)pcWriteBuffer,
//                                           xWriteBufferLen - 1,
//                                           pdMS_TO_TICKS(WAIT_TIME));
//
//     if (xReceivedBytes > 0) {
//         /* Add null terminator so the CLI can print it */
//         pcWriteBuffer[xReceivedBytes] = '\0';
//     } else {
//         /* If 0 bytes received in 500ms, indicate timeout */
//         strncpy(pcWriteBuffer, "Error: UART Timeout\r\n", xWriteBufferLen);
//     }
//
//     return pdFALSE;
// }

void Register_CLI_Commands(void) {
  FreeRTOS_CLIRegisterCommand(&xRGBCommand);
  FreeRTOS_CLIRegisterCommand(&xUARTQueryCommand);
}
