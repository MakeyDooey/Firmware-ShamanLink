#ifndef _CLI_COMMANDS_H_
#define _CLI_COMMANDS_H_

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @brief Registers all application-specific FreeRTOS CLI commands.
 */
void Register_CLI_Commands(void);

#ifdef __cplusplus
}
#endif

#endif /* _CLI_COMMANDS_H_ */
