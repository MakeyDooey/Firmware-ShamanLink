#include "cli_commands.h"
#include "FreeRTOS.h"
#include "FreeRTOS_CLI.h"
#include "stream_buffer.h" // Required for xStreamBufferReceive
#include "fsl_usart.h"     // Required for USART_WriteBlocking
#include "board.h"
#include "task.h"
#include <string.h>
#include <stdio.h>

#define WAIT_TIME 2000

/* Tell the compiler this lives in another file */
extern volatile bool g_uartResponseMapping;
extern StreamBufferHandle_t xUart0RxStream;
/* Extern the handle created in your main/hardware init */
extern StreamBufferHandle_t xUart0RxStream;

/* RGB command definition */
static BaseType_t prvRGBCommand(char *pcWriteBuffer, size_t xWriteBufferLen,
                                const char *pcCommandString);

/* UART Query command definition */
static BaseType_t prvUARTCommand(char *pcWriteBuffer, size_t xWriteBufferLen,
                                const char *pcCommandString);

static const CLI_Command_Definition_t xRGBCommand = {
    "rgb",
    "\r\nrgb <color>:\r\n Set RGB LED color (e.g. 'rgb red', 'rgb blue')\r\n",
    prvRGBCommand, 1
};

static const CLI_Command_Definition_t xUARTQueryCommand = {
    "query",
    "\r\nquery <str>:\r\n Sends <str> to UART0 and waits 500ms for a response\r\n",
    prvUARTCommand, 1
};

/* --- RGB Command Handler --- */
static BaseType_t prvRGBCommand(char *pcWriteBuffer, size_t xWriteBufferLen,
                                const char *pcCommandString) {
    const char *pcParameter;
    BaseType_t xParameterStringLength;

    pcParameter = FreeRTOS_CLIGetParameter(pcCommandString, 1, &xParameterStringLength);

    if (pcParameter != NULL) {
        if (strncmp(pcParameter, "red", xParameterStringLength) == 0 && xParameterStringLength == 3) {
            LED_RED_ON(); LED_BLUE_OFF(); LED_GREEN_OFF();
            strncpy(pcWriteBuffer, "Red LED ON\r\n", xWriteBufferLen);
        } else if (strncmp(pcParameter, "blue", xParameterStringLength) == 0 && xParameterStringLength == 4) {
            LED_RED_OFF(); LED_BLUE_ON(); LED_GREEN_OFF();
            strncpy(pcWriteBuffer, "Blue LED ON\r\n", xWriteBufferLen);
        } else if (strncmp(pcParameter, "green", xParameterStringLength) == 0 && xParameterStringLength == 5) {
            LED_RED_OFF(); LED_BLUE_OFF(); LED_GREEN_ON();
            strncpy(pcWriteBuffer, "Green LED ON\r\n", xWriteBufferLen);
        } else if (strncmp(pcParameter, "off", xParameterStringLength) == 0 && xParameterStringLength == 3) {
            LED_RED_OFF(); LED_BLUE_OFF(); LED_GREEN_OFF();
            strncpy(pcWriteBuffer, "LEDs OFF\r\n", xWriteBufferLen);
        } else {
            strncpy(pcWriteBuffer, "Unknown color. Red/Green/Blue/Off supported.\r\n", xWriteBufferLen);
        }
    } else {
        strncpy(pcWriteBuffer, "Missing parameter.\r\n", xWriteBufferLen);
    }
    return pdFALSE;
}
/* --- UART Query Command Handler --- */
static BaseType_t prvUARTCommand(char *pcWriteBuffer, size_t xWriteBufferLen,
                                const char *pcCommandString) {
    const char *pcParameter;
    BaseType_t xParameterStringLength;
    size_t xTotalBytes = 0;
    uint8_t ucRxByte;

    /* 1. Extract the string to send (e.g., "HELLO") */
    pcParameter = FreeRTOS_CLIGetParameter(pcCommandString, 1, &xParameterStringLength);

    if (pcParameter == NULL) {
        strncpy(pcWriteBuffer, "Usage: query <string>\r\n", xWriteBufferLen);
        return pdFALSE;
    }

    /* 2. Prepare for Response: Clear old data and flip the ISR switch */
    xStreamBufferReset(xUart0RxStream);
    g_uartResponseMapping = true;

    /* 3. Send the command + single newline to ESP32 */
    USART_WriteBlocking(USART0, (uint8_t *)pcParameter, xParameterStringLength);
    USART_WriteBlocking(USART0, (uint8_t *)"\n", 1);

    /* 4. The "Collection Loop":
          Wait for bytes one-by-one until we see a newline or hit the timeout.
          This keeps the 'g_uartResponseMapping' flag active for the WHOLE word. */
    while (xTotalBytes < (xWriteBufferLen - 2)) { // Leave room for null terminator
        // Wait up to WAIT_TIME for EACH byte
        size_t xReceived = xStreamBufferReceive(xUart0RxStream,
                                               &ucRxByte, 1,
                                               pdMS_TO_TICKS(WAIT_TIME));

        if (xReceived > 0) {
            pcWriteBuffer[xTotalBytes++] = (char)ucRxByte;

            /* Stop if we hit the end of the line from ESP32 */
            if (ucRxByte == '\n' || ucRxByte == '\r') {
                break;
            }
        } else {
            /* No data arrived within the timeout period */
            break;
        }
    }

    /* 5. flip the switch back.
          The tunnel is now closed; UART traffic returns to the CLI. */
    g_uartResponseMapping = false;

    /* 6. Format the output for the user */
        if (xTotalBytes > 0) {
            /* Remove trailing newlines/carriage returns from the ESP32 response
               to prevent double-spacing and double-prompts. */
            while (xTotalBytes > 0 &&
                  (pcWriteBuffer[xTotalBytes - 1] == '\n' || pcWriteBuffer[xTotalBytes - 1] == '\r')) {
                xTotalBytes--;
            }
            pcWriteBuffer[xTotalBytes] = '\0'; // Re-terminate after trimming

            /* --- THE CLEAN VT100 SEQUENCE ---
             * \033[1A : Move up to the line where 'query' was typed
             * \033[K  : Clear that line
             * [TARGET]: The actual data
             * (Note: We do NOT add a trailing \n here because the CLI Task
             * will provide the newline and the '>' prompt immediately after we return.)
             */
            char tempBuf[configCOMMAND_INT_MAX_OUTPUT_SIZE];
            snprintf(tempBuf, sizeof(tempBuf), "[TARGET]: %s", pcWriteBuffer);

            strncpy(pcWriteBuffer, tempBuf, xWriteBufferLen);
        }

    return pdFALSE;
}

///* --- UART Query Command Handler --- */
//static BaseType_t prvUARTCommand(char *pcWriteBuffer, size_t xWriteBufferLen,
//                                const char *pcCommandString) {
//    const char *pcParameter;
//    BaseType_t xParameterStringLength;
//    size_t xReceivedBytes;
//
//    /* 1. Extract the string to send */
//    pcParameter = FreeRTOS_CLIGetParameter(pcCommandString, 1, &xParameterStringLength);
//
//    if (pcParameter == NULL) {
//        strncpy(pcWriteBuffer, "Usage: query <string>\r\n", xWriteBufferLen);
//        return pdFALSE;
//    }
//
//    /* 2. Flush any old data sitting in the stream buffer */
//    xStreamBufferReset(xUart0RxStream);
//
//    /* 3. Send the string + a newline to UART0 */
//    USART_WriteBlocking(USART0, (uint8_t *)pcParameter, xParameterStringLength);
//    USART_WriteBlocking(USART0, (uint8_t *)"\r\n", 2);
//
//    /* 4. Block and wait for a response.
//          pcWriteBuffer is used as the temporary storage for the RX data. */
//    xReceivedBytes = xStreamBufferReceive(xUart0RxStream,
//                                          (void *)pcWriteBuffer,
//                                          xWriteBufferLen - 1,
//                                          pdMS_TO_TICKS(WAIT_TIME));
//
//    if (xReceivedBytes > 0) {
//        /* Add null terminator so the CLI can print it */
//        pcWriteBuffer[xReceivedBytes] = '\0';
//    } else {
//        /* If 0 bytes received in 500ms, indicate timeout */
//        strncpy(pcWriteBuffer, "Error: UART Timeout\r\n", xWriteBufferLen);
//    }
//
//    return pdFALSE;
//}

void Register_CLI_Commands(void) {
    FreeRTOS_CLIRegisterCommand(&xRGBCommand);
    FreeRTOS_CLIRegisterCommand(&xUARTQueryCommand);
}
