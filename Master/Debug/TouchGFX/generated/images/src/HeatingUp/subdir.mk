################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../TouchGFX/generated/images/src/HeatingUp/image_heating_up_bar.cpp 

OBJS += \
./TouchGFX/generated/images/src/HeatingUp/image_heating_up_bar.o 

CPP_DEPS += \
./TouchGFX/generated/images/src/HeatingUp/image_heating_up_bar.d 


# Each subdirectory must supply rules for building sources it contributes
TouchGFX/generated/images/src/HeatingUp/%.o TouchGFX/generated/images/src/HeatingUp/%.su TouchGFX/generated/images/src/HeatingUp/%.cyclo: ../TouchGFX/generated/images/src/HeatingUp/%.cpp TouchGFX/generated/images/src/HeatingUp/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m7 -std=gnu++14 -g3 -DDEBUG -DUSE_PWR_LDO_SUPPLY -DUSE_NUCLEO_64 -DUSE_HAL_DRIVER -DSTM32H723xx -c -I../Core/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/BSP/STM32H7xx_Nucleo -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -I../TouchGFX/App -I../TouchGFX/target/generated -I../TouchGFX/target -I../Middlewares/ST/touchgfx/framework/include -I../TouchGFX/generated/fonts/include -I../TouchGFX/generated/gui_generated/include -I../TouchGFX/generated/images/include -I../TouchGFX/generated/texts/include -I../TouchGFX/generated/videos/include -I../TouchGFX/gui/include -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -femit-class-debug-always -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-TouchGFX-2f-generated-2f-images-2f-src-2f-HeatingUp

clean-TouchGFX-2f-generated-2f-images-2f-src-2f-HeatingUp:
	-$(RM) ./TouchGFX/generated/images/src/HeatingUp/image_heating_up_bar.cyclo ./TouchGFX/generated/images/src/HeatingUp/image_heating_up_bar.d ./TouchGFX/generated/images/src/HeatingUp/image_heating_up_bar.o ./TouchGFX/generated/images/src/HeatingUp/image_heating_up_bar.su

.PHONY: clean-TouchGFX-2f-generated-2f-images-2f-src-2f-HeatingUp

