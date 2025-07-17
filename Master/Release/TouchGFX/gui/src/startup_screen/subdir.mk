################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../TouchGFX/gui/src/startup_screen/StartupPresenter.cpp \
../TouchGFX/gui/src/startup_screen/StartupView.cpp 

OBJS += \
./TouchGFX/gui/src/startup_screen/StartupPresenter.o \
./TouchGFX/gui/src/startup_screen/StartupView.o 

CPP_DEPS += \
./TouchGFX/gui/src/startup_screen/StartupPresenter.d \
./TouchGFX/gui/src/startup_screen/StartupView.d 


# Each subdirectory must supply rules for building sources it contributes
TouchGFX/gui/src/startup_screen/%.o TouchGFX/gui/src/startup_screen/%.su TouchGFX/gui/src/startup_screen/%.cyclo: ../TouchGFX/gui/src/startup_screen/%.cpp TouchGFX/gui/src/startup_screen/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m7 -std=gnu++14 -DUSE_PWR_LDO_SUPPLY -DUSE_NUCLEO_64 -DUSE_HAL_DRIVER -DSTM32H723xx -c -I../Core/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/BSP/STM32H7xx_Nucleo -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -I../TouchGFX/App -I../TouchGFX/target/generated -I../TouchGFX/target -I../Middlewares/ST/touchgfx/framework/include -I../TouchGFX/generated/fonts/include -I../TouchGFX/generated/gui_generated/include -I../TouchGFX/generated/images/include -I../TouchGFX/generated/texts/include -I../TouchGFX/generated/videos/include -I../TouchGFX/gui/include -Os -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -femit-class-debug-always -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-TouchGFX-2f-gui-2f-src-2f-startup_screen

clean-TouchGFX-2f-gui-2f-src-2f-startup_screen:
	-$(RM) ./TouchGFX/gui/src/startup_screen/StartupPresenter.cyclo ./TouchGFX/gui/src/startup_screen/StartupPresenter.d ./TouchGFX/gui/src/startup_screen/StartupPresenter.o ./TouchGFX/gui/src/startup_screen/StartupPresenter.su ./TouchGFX/gui/src/startup_screen/StartupView.cyclo ./TouchGFX/gui/src/startup_screen/StartupView.d ./TouchGFX/gui/src/startup_screen/StartupView.o ./TouchGFX/gui/src/startup_screen/StartupView.su

.PHONY: clean-TouchGFX-2f-gui-2f-src-2f-startup_screen

