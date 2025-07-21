################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../TouchGFX/generated/gui_generated/src/startup_screen/StartupViewBase.cpp 

OBJS += \
./TouchGFX/generated/gui_generated/src/startup_screen/StartupViewBase.o 

CPP_DEPS += \
./TouchGFX/generated/gui_generated/src/startup_screen/StartupViewBase.d 


# Each subdirectory must supply rules for building sources it contributes
TouchGFX/generated/gui_generated/src/startup_screen/%.o TouchGFX/generated/gui_generated/src/startup_screen/%.su TouchGFX/generated/gui_generated/src/startup_screen/%.cyclo: ../TouchGFX/generated/gui_generated/src/startup_screen/%.cpp TouchGFX/generated/gui_generated/src/startup_screen/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m7 -std=gnu++14 -g3 -DDEBUG -DUSE_PWR_LDO_SUPPLY -DUSE_HAL_DRIVER -DSTM32H723xx -c -I../Core/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/ST/touchgfx/framework/include -I../TouchGFX/generated/fonts/include -I../TouchGFX/generated/gui_generated/include -I../TouchGFX/generated/images/include -I../TouchGFX/generated/texts/include -I../TouchGFX/generated/videos/include -I../TouchGFX/gui/include -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -femit-class-debug-always -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-TouchGFX-2f-generated-2f-gui_generated-2f-src-2f-startup_screen

clean-TouchGFX-2f-generated-2f-gui_generated-2f-src-2f-startup_screen:
	-$(RM) ./TouchGFX/generated/gui_generated/src/startup_screen/StartupViewBase.cyclo ./TouchGFX/generated/gui_generated/src/startup_screen/StartupViewBase.d ./TouchGFX/generated/gui_generated/src/startup_screen/StartupViewBase.o ./TouchGFX/generated/gui_generated/src/startup_screen/StartupViewBase.su

.PHONY: clean-TouchGFX-2f-generated-2f-gui_generated-2f-src-2f-startup_screen

