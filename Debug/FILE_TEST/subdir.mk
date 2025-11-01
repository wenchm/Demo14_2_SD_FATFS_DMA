################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../FILE_TEST/file_opera.c 

OBJS += \
./FILE_TEST/file_opera.o 

C_DEPS += \
./FILE_TEST/file_opera.d 


# Each subdirectory must supply rules for building sources it contributes
FILE_TEST/%.o FILE_TEST/%.su FILE_TEST/%.cyclo: ../FILE_TEST/%.c FILE_TEST/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -I"C:/Users/YCZN_MT/STM32CubeIDE/workspace_1.19.0/Demo14_2_SD_FATFS_DMA/FILE_TEST" -I"C:/Users/YCZN_MT/STM32CubeIDE/workspace_1.19.0/Demo14_2_SD_FATFS_DMA/KEY_LED" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-FILE_TEST

clean-FILE_TEST:
	-$(RM) ./FILE_TEST/file_opera.cyclo ./FILE_TEST/file_opera.d ./FILE_TEST/file_opera.o ./FILE_TEST/file_opera.su

.PHONY: clean-FILE_TEST

