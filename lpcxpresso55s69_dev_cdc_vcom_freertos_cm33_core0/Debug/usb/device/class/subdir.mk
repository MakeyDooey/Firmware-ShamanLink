################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../usb/device/class/usb_device_cdc_acm.c \
../usb/device/class/usb_device_class.c 

C_DEPS += \
./usb/device/class/usb_device_cdc_acm.d \
./usb/device/class/usb_device_class.d 

OBJS += \
./usb/device/class/usb_device_cdc_acm.o \
./usb/device/class/usb_device_class.o 


# Each subdirectory must supply rules for building sources it contributes
usb/device/class/%.o: ../usb/device/class/%.c usb/device/class/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=gnu99 -D__REDLIB__ -DMCUXPRESSO_SDK -DUSB_STACK_FREERTOS -DUSB_STACK_USE_DEDICATED_RAM=1 -DSDK_DEBUGCONSOLE=1 -DMCUX_META_BUILD -DLPC55S69_cm33_core0_SERIES -DOSA_USED -DUSE_RTOS=1 -DSDK_OS_FREE_RTOS -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -DCPU_LPC55S69JBD64 -DCPU_LPC55S69JBD64_cm33 -DCPU_LPC55S69JBD64_cm33_core0 -I"C:\Users\MD\Documents\MCUXpressoIDE_25.6.136\workspace\lpcxpresso55s69_dev_cdc_vcom_freertos_cm33_core0\source" -I"C:\Users\MD\Documents\MCUXpressoIDE_25.6.136\workspace\lpcxpresso55s69_dev_cdc_vcom_freertos_cm33_core0\drivers" -I"C:\Users\MD\Documents\MCUXpressoIDE_25.6.136\workspace\lpcxpresso55s69_dev_cdc_vcom_freertos_cm33_core0\CMSIS" -I"C:\Users\MD\Documents\MCUXpressoIDE_25.6.136\workspace\lpcxpresso55s69_dev_cdc_vcom_freertos_cm33_core0\CMSIS\m-profile" -I"C:\Users\MD\Documents\MCUXpressoIDE_25.6.136\workspace\lpcxpresso55s69_dev_cdc_vcom_freertos_cm33_core0\device" -I"C:\Users\MD\Documents\MCUXpressoIDE_25.6.136\workspace\lpcxpresso55s69_dev_cdc_vcom_freertos_cm33_core0\device\periph" -I"C:\Users\MD\Documents\MCUXpressoIDE_25.6.136\workspace\lpcxpresso55s69_dev_cdc_vcom_freertos_cm33_core0\utilities" -I"C:\Users\MD\Documents\MCUXpressoIDE_25.6.136\workspace\lpcxpresso55s69_dev_cdc_vcom_freertos_cm33_core0\component\lists" -I"C:\Users\MD\Documents\MCUXpressoIDE_25.6.136\workspace\lpcxpresso55s69_dev_cdc_vcom_freertos_cm33_core0\utilities\str" -I"C:\Users\MD\Documents\MCUXpressoIDE_25.6.136\workspace\lpcxpresso55s69_dev_cdc_vcom_freertos_cm33_core0\utilities\debug_console_lite" -I"C:\Users\MD\Documents\MCUXpressoIDE_25.6.136\workspace\lpcxpresso55s69_dev_cdc_vcom_freertos_cm33_core0\component\uart" -I"C:\Users\MD\Documents\MCUXpressoIDE_25.6.136\workspace\lpcxpresso55s69_dev_cdc_vcom_freertos_cm33_core0\component\osa\config" -I"C:\Users\MD\Documents\MCUXpressoIDE_25.6.136\workspace\lpcxpresso55s69_dev_cdc_vcom_freertos_cm33_core0\component\osa" -I"C:\Users\MD\Documents\MCUXpressoIDE_25.6.136\workspace\lpcxpresso55s69_dev_cdc_vcom_freertos_cm33_core0\usb\include" -I"C:\Users\MD\Documents\MCUXpressoIDE_25.6.136\workspace\lpcxpresso55s69_dev_cdc_vcom_freertos_cm33_core0\usb\device" -I"C:\Users\MD\Documents\MCUXpressoIDE_25.6.136\workspace\lpcxpresso55s69_dev_cdc_vcom_freertos_cm33_core0\usb\device\class" -I"C:\Users\MD\Documents\MCUXpressoIDE_25.6.136\workspace\lpcxpresso55s69_dev_cdc_vcom_freertos_cm33_core0\usb\phy" -I"C:\Users\MD\Documents\MCUXpressoIDE_25.6.136\workspace\lpcxpresso55s69_dev_cdc_vcom_freertos_cm33_core0\freertos\freertos-kernel\include" -I"C:\Users\MD\Documents\MCUXpressoIDE_25.6.136\workspace\lpcxpresso55s69_dev_cdc_vcom_freertos_cm33_core0\freertos\freertos-kernel\portable\GCC\ARM_CM33_NTZ\non_secure" -I"C:\Users\MD\Documents\MCUXpressoIDE_25.6.136\workspace\lpcxpresso55s69_dev_cdc_vcom_freertos_cm33_core0\board" -I"C:\Users\MD\Documents\MCUXpressoIDE_25.6.136\workspace\lpcxpresso55s69_dev_cdc_vcom_freertos_cm33_core0\source\config\device\ip3511fs" -I"C:\Users\MD\Documents\MCUXpressoIDE_25.6.136\workspace\lpcxpresso55s69_dev_cdc_vcom_freertos_cm33_core0\source\config\device\ip3511hs" -I"C:\Users\MD\Documents\MCUXpressoIDE_25.6.136\workspace\lpcxpresso55s69_dev_cdc_vcom_freertos_cm33_core0\freertos\freertos-kernel\template" -I"C:\Users\MD\Documents\MCUXpressoIDE_25.6.136\workspace\lpcxpresso55s69_dev_cdc_vcom_freertos_cm33_core0\freertos\freertos-kernel\template\ARM_CM33_3_priority_bits" -O0 -fno-common -g3 -gdwarf-4 -mno-unaligned-access -mcpu=cortex-m33 -c -ffunction-sections -fdata-sections -fno-builtin -imacros "C:\Users\MD\Documents\MCUXpressoIDE_25.6.136\workspace\lpcxpresso55s69_dev_cdc_vcom_freertos_cm33_core0\source\mcux_config.h" -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m33 -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-usb-2f-device-2f-class

clean-usb-2f-device-2f-class:
	-$(RM) ./usb/device/class/usb_device_cdc_acm.d ./usb/device/class/usb_device_cdc_acm.o ./usb/device/class/usb_device_class.d ./usb/device/class/usb_device_class.o

.PHONY: clean-usb-2f-device-2f-class

