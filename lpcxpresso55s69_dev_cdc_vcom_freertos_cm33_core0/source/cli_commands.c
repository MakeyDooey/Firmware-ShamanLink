#include "cli_commands.h"
#include "FreeRTOS.h"
#include "FreeRTOS_CLI.h"
#include "board.h"
#include "task.h"
#include <string.h>

/* RGB command definition */
static BaseType_t prvRGBCommand(char *pcWriteBuffer, size_t xWriteBufferLen,
                                const char *pcCommandString);

static const CLI_Command_Definition_t xRGBCommand = {
    "rgb",
    "\r\nrgb:\r\n Set RGB LED color (e.g. 'rgb red', 'rgb blue')\r\n\r\n",
    prvRGBCommand, 1 /* One parameter expected */
};

/*!
 * @brief RGB command handler implementation.
 */
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

void Register_CLI_Commands(void) { FreeRTOS_CLIRegisterCommand(&xRGBCommand); }
