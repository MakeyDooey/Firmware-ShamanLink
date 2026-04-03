################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../drivers/fsl_clock.c \
../drivers/fsl_common.c \
../drivers/fsl_common_arm.c \
../drivers/fsl_flexcomm.c \
../drivers/fsl_gpio.c \
../drivers/fsl_i2c.c \
../drivers/fsl_power.c \
../drivers/fsl_reset.c \
../drivers/fsl_spi.c \
../drivers/fsl_usart.c 

C_DEPS += \
./drivers/fsl_clock.d \
./drivers/fsl_common.d \
./drivers/fsl_common_arm.d \
./drivers/fsl_flexcomm.d \
./drivers/fsl_gpio.d \
./drivers/fsl_i2c.d \
./drivers/fsl_power.d \
./drivers/fsl_reset.d \
./drivers/fsl_spi.d \
./drivers/fsl_usart.d 

OBJS += \
./drivers/fsl_clock.o \
./drivers/fsl_common.o \
./drivers/fsl_common_arm.o \
./drivers/fsl_flexcomm.o \
./drivers/fsl_gpio.o \
./drivers/fsl_i2c.o \
./drivers/fsl_power.o \
./drivers/fsl_reset.o \
./drivers/fsl_spi.o \
./drivers/fsl_usart.o 


# Each subdirectory must supply rules for building sources it contributes
drivers/%.o: ../drivers/%.c drivers/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=gnu99 -D__REDLIB__ -DCPU_LPC55S69JBD100 -DCPU_LPC55S69JBD100_cm33 -DCPU_LPC55S69JBD100_cm33_core0 -DMCUXPRESSO_SDK -DSDK_DEBUGCONSOLE=1 -DMCUX_META_BUILD -DLPC55S69_cm33_core0_SERIES -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -I"C:\Users\MD\Documents\MCUXpressoIDE_25.6.136\workspace\lpcxpresso55s69_led_blinky_lpc_cm33_core0\source" -I"C:\Users\MD\Documents\MCUXpressoIDE_25.6.136\workspace\lpcxpresso55s69_led_blinky_lpc_cm33_core0\drivers" -I"C:\Users\MD\Documents\MCUXpressoIDE_25.6.136\workspace\lpcxpresso55s69_led_blinky_lpc_cm33_core0\CMSIS" -I"C:\Users\MD\Documents\MCUXpressoIDE_25.6.136\workspace\lpcxpresso55s69_led_blinky_lpc_cm33_core0\CMSIS\m-profile" -I"C:\Users\MD\Documents\MCUXpressoIDE_25.6.136\workspace\lpcxpresso55s69_led_blinky_lpc_cm33_core0\device" -I"C:\Users\MD\Documents\MCUXpressoIDE_25.6.136\workspace\lpcxpresso55s69_led_blinky_lpc_cm33_core0\device\periph" -I"C:\Users\MD\Documents\MCUXpressoIDE_25.6.136\workspace\lpcxpresso55s69_led_blinky_lpc_cm33_core0\utilities" -I"C:\Users\MD\Documents\MCUXpressoIDE_25.6.136\workspace\lpcxpresso55s69_led_blinky_lpc_cm33_core0\utilities\str" -I"C:\Users\MD\Documents\MCUXpressoIDE_25.6.136\workspace\lpcxpresso55s69_led_blinky_lpc_cm33_core0\utilities\debug_console_lite" -I"C:\Users\MD\Documents\MCUXpressoIDE_25.6.136\workspace\lpcxpresso55s69_led_blinky_lpc_cm33_core0\component\uart" -I"C:\Users\MD\Documents\MCUXpressoIDE_25.6.136\workspace\lpcxpresso55s69_led_blinky_lpc_cm33_core0\board" -O0 -fno-common -g3 -gdwarf-4 -mcpu=cortex-m33 -c -ffunction-sections -fdata-sections -fno-builtin -imacros "C:\Users\MD\Documents\MCUXpressoIDE_25.6.136\workspace\lpcxpresso55s69_led_blinky_lpc_cm33_core0\source\mcux_config.h" -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m33 -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-drivers

clean-drivers:
	-$(RM) ./drivers/fsl_clock.d ./drivers/fsl_clock.o ./drivers/fsl_common.d ./drivers/fsl_common.o ./drivers/fsl_common_arm.d ./drivers/fsl_common_arm.o ./drivers/fsl_flexcomm.d ./drivers/fsl_flexcomm.o ./drivers/fsl_gpio.d ./drivers/fsl_gpio.o ./drivers/fsl_i2c.d ./drivers/fsl_i2c.o ./drivers/fsl_power.d ./drivers/fsl_power.o ./drivers/fsl_reset.d ./drivers/fsl_reset.o ./drivers/fsl_spi.d ./drivers/fsl_spi.o ./drivers/fsl_usart.d ./drivers/fsl_usart.o

.PHONY: clean-drivers

