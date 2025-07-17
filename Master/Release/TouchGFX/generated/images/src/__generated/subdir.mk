################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../TouchGFX/generated/images/src/__generated/image_alternate_theme_images_widgets_button_icon_round_micro_fill_normal.cpp \
../TouchGFX/generated/images/src/__generated/image_alternate_theme_images_widgets_button_icon_round_micro_fill_pressed.cpp \
../TouchGFX/generated/images/src/__generated/image_alternate_theme_images_widgets_button_icon_round_tiny_fill_normal.cpp \
../TouchGFX/generated/images/src/__generated/image_alternate_theme_images_widgets_button_icon_round_tiny_fill_pressed.cpp \
../TouchGFX/generated/images/src/__generated/image_alternate_theme_images_widgets_button_icon_rounded_tiny_fill_normal.cpp \
../TouchGFX/generated/images/src/__generated/image_alternate_theme_images_widgets_button_icon_rounded_tiny_fill_pressed.cpp \
../TouchGFX/generated/images/src/__generated/image_icon_theme_images_action_done_40_40_E8F6FB.svg.cpp \
../TouchGFX/generated/images/src/__generated/image_icon_theme_images_action_settings_applications_50_50_E8F6FB.svg.cpp \
../TouchGFX/generated/images/src/__generated/image_icon_theme_images_editor_mode_edit_25_25_E8F6FB.svg.cpp \
../TouchGFX/generated/images/src/__generated/image_icon_theme_images_navigation_arrow_back_ios_50_50_E8F6FB.svg.cpp \
../TouchGFX/generated/images/src/__generated/image_icon_theme_images_navigation_cancel_40_40_E8F6FB.svg.cpp 

OBJS += \
./TouchGFX/generated/images/src/__generated/image_alternate_theme_images_widgets_button_icon_round_micro_fill_normal.o \
./TouchGFX/generated/images/src/__generated/image_alternate_theme_images_widgets_button_icon_round_micro_fill_pressed.o \
./TouchGFX/generated/images/src/__generated/image_alternate_theme_images_widgets_button_icon_round_tiny_fill_normal.o \
./TouchGFX/generated/images/src/__generated/image_alternate_theme_images_widgets_button_icon_round_tiny_fill_pressed.o \
./TouchGFX/generated/images/src/__generated/image_alternate_theme_images_widgets_button_icon_rounded_tiny_fill_normal.o \
./TouchGFX/generated/images/src/__generated/image_alternate_theme_images_widgets_button_icon_rounded_tiny_fill_pressed.o \
./TouchGFX/generated/images/src/__generated/image_icon_theme_images_action_done_40_40_E8F6FB.svg.o \
./TouchGFX/generated/images/src/__generated/image_icon_theme_images_action_settings_applications_50_50_E8F6FB.svg.o \
./TouchGFX/generated/images/src/__generated/image_icon_theme_images_editor_mode_edit_25_25_E8F6FB.svg.o \
./TouchGFX/generated/images/src/__generated/image_icon_theme_images_navigation_arrow_back_ios_50_50_E8F6FB.svg.o \
./TouchGFX/generated/images/src/__generated/image_icon_theme_images_navigation_cancel_40_40_E8F6FB.svg.o 

CPP_DEPS += \
./TouchGFX/generated/images/src/__generated/image_alternate_theme_images_widgets_button_icon_round_micro_fill_normal.d \
./TouchGFX/generated/images/src/__generated/image_alternate_theme_images_widgets_button_icon_round_micro_fill_pressed.d \
./TouchGFX/generated/images/src/__generated/image_alternate_theme_images_widgets_button_icon_round_tiny_fill_normal.d \
./TouchGFX/generated/images/src/__generated/image_alternate_theme_images_widgets_button_icon_round_tiny_fill_pressed.d \
./TouchGFX/generated/images/src/__generated/image_alternate_theme_images_widgets_button_icon_rounded_tiny_fill_normal.d \
./TouchGFX/generated/images/src/__generated/image_alternate_theme_images_widgets_button_icon_rounded_tiny_fill_pressed.d \
./TouchGFX/generated/images/src/__generated/image_icon_theme_images_action_done_40_40_E8F6FB.svg.d \
./TouchGFX/generated/images/src/__generated/image_icon_theme_images_action_settings_applications_50_50_E8F6FB.svg.d \
./TouchGFX/generated/images/src/__generated/image_icon_theme_images_editor_mode_edit_25_25_E8F6FB.svg.d \
./TouchGFX/generated/images/src/__generated/image_icon_theme_images_navigation_arrow_back_ios_50_50_E8F6FB.svg.d \
./TouchGFX/generated/images/src/__generated/image_icon_theme_images_navigation_cancel_40_40_E8F6FB.svg.d 


# Each subdirectory must supply rules for building sources it contributes
TouchGFX/generated/images/src/__generated/%.o TouchGFX/generated/images/src/__generated/%.su TouchGFX/generated/images/src/__generated/%.cyclo: ../TouchGFX/generated/images/src/__generated/%.cpp TouchGFX/generated/images/src/__generated/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m7 -std=gnu++14 -DUSE_PWR_LDO_SUPPLY -DUSE_NUCLEO_64 -DUSE_HAL_DRIVER -DSTM32H723xx -c -I../Core/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/BSP/STM32H7xx_Nucleo -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -I../TouchGFX/App -I../TouchGFX/target/generated -I../TouchGFX/target -I../Middlewares/ST/touchgfx/framework/include -I../TouchGFX/generated/fonts/include -I../TouchGFX/generated/gui_generated/include -I../TouchGFX/generated/images/include -I../TouchGFX/generated/texts/include -I../TouchGFX/generated/videos/include -I../TouchGFX/gui/include -Os -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -femit-class-debug-always -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-TouchGFX-2f-generated-2f-images-2f-src-2f-__generated

clean-TouchGFX-2f-generated-2f-images-2f-src-2f-__generated:
	-$(RM) ./TouchGFX/generated/images/src/__generated/image_alternate_theme_images_widgets_button_icon_round_micro_fill_normal.cyclo ./TouchGFX/generated/images/src/__generated/image_alternate_theme_images_widgets_button_icon_round_micro_fill_normal.d ./TouchGFX/generated/images/src/__generated/image_alternate_theme_images_widgets_button_icon_round_micro_fill_normal.o ./TouchGFX/generated/images/src/__generated/image_alternate_theme_images_widgets_button_icon_round_micro_fill_normal.su ./TouchGFX/generated/images/src/__generated/image_alternate_theme_images_widgets_button_icon_round_micro_fill_pressed.cyclo ./TouchGFX/generated/images/src/__generated/image_alternate_theme_images_widgets_button_icon_round_micro_fill_pressed.d ./TouchGFX/generated/images/src/__generated/image_alternate_theme_images_widgets_button_icon_round_micro_fill_pressed.o ./TouchGFX/generated/images/src/__generated/image_alternate_theme_images_widgets_button_icon_round_micro_fill_pressed.su ./TouchGFX/generated/images/src/__generated/image_alternate_theme_images_widgets_button_icon_round_tiny_fill_normal.cyclo ./TouchGFX/generated/images/src/__generated/image_alternate_theme_images_widgets_button_icon_round_tiny_fill_normal.d ./TouchGFX/generated/images/src/__generated/image_alternate_theme_images_widgets_button_icon_round_tiny_fill_normal.o ./TouchGFX/generated/images/src/__generated/image_alternate_theme_images_widgets_button_icon_round_tiny_fill_normal.su ./TouchGFX/generated/images/src/__generated/image_alternate_theme_images_widgets_button_icon_round_tiny_fill_pressed.cyclo ./TouchGFX/generated/images/src/__generated/image_alternate_theme_images_widgets_button_icon_round_tiny_fill_pressed.d ./TouchGFX/generated/images/src/__generated/image_alternate_theme_images_widgets_button_icon_round_tiny_fill_pressed.o ./TouchGFX/generated/images/src/__generated/image_alternate_theme_images_widgets_button_icon_round_tiny_fill_pressed.su ./TouchGFX/generated/images/src/__generated/image_alternate_theme_images_widgets_button_icon_rounded_tiny_fill_normal.cyclo ./TouchGFX/generated/images/src/__generated/image_alternate_theme_images_widgets_button_icon_rounded_tiny_fill_normal.d ./TouchGFX/generated/images/src/__generated/image_alternate_theme_images_widgets_button_icon_rounded_tiny_fill_normal.o ./TouchGFX/generated/images/src/__generated/image_alternate_theme_images_widgets_button_icon_rounded_tiny_fill_normal.su ./TouchGFX/generated/images/src/__generated/image_alternate_theme_images_widgets_button_icon_rounded_tiny_fill_pressed.cyclo ./TouchGFX/generated/images/src/__generated/image_alternate_theme_images_widgets_button_icon_rounded_tiny_fill_pressed.d ./TouchGFX/generated/images/src/__generated/image_alternate_theme_images_widgets_button_icon_rounded_tiny_fill_pressed.o ./TouchGFX/generated/images/src/__generated/image_alternate_theme_images_widgets_button_icon_rounded_tiny_fill_pressed.su ./TouchGFX/generated/images/src/__generated/image_icon_theme_images_action_done_40_40_E8F6FB.svg.cyclo ./TouchGFX/generated/images/src/__generated/image_icon_theme_images_action_done_40_40_E8F6FB.svg.d ./TouchGFX/generated/images/src/__generated/image_icon_theme_images_action_done_40_40_E8F6FB.svg.o ./TouchGFX/generated/images/src/__generated/image_icon_theme_images_action_done_40_40_E8F6FB.svg.su ./TouchGFX/generated/images/src/__generated/image_icon_theme_images_action_settings_applications_50_50_E8F6FB.svg.cyclo ./TouchGFX/generated/images/src/__generated/image_icon_theme_images_action_settings_applications_50_50_E8F6FB.svg.d ./TouchGFX/generated/images/src/__generated/image_icon_theme_images_action_settings_applications_50_50_E8F6FB.svg.o ./TouchGFX/generated/images/src/__generated/image_icon_theme_images_action_settings_applications_50_50_E8F6FB.svg.su ./TouchGFX/generated/images/src/__generated/image_icon_theme_images_editor_mode_edit_25_25_E8F6FB.svg.cyclo ./TouchGFX/generated/images/src/__generated/image_icon_theme_images_editor_mode_edit_25_25_E8F6FB.svg.d ./TouchGFX/generated/images/src/__generated/image_icon_theme_images_editor_mode_edit_25_25_E8F6FB.svg.o ./TouchGFX/generated/images/src/__generated/image_icon_theme_images_editor_mode_edit_25_25_E8F6FB.svg.su ./TouchGFX/generated/images/src/__generated/image_icon_theme_images_navigation_arrow_back_ios_50_50_E8F6FB.svg.cyclo ./TouchGFX/generated/images/src/__generated/image_icon_theme_images_navigation_arrow_back_ios_50_50_E8F6FB.svg.d ./TouchGFX/generated/images/src/__generated/image_icon_theme_images_navigation_arrow_back_ios_50_50_E8F6FB.svg.o ./TouchGFX/generated/images/src/__generated/image_icon_theme_images_navigation_arrow_back_ios_50_50_E8F6FB.svg.su ./TouchGFX/generated/images/src/__generated/image_icon_theme_images_navigation_cancel_40_40_E8F6FB.svg.cyclo ./TouchGFX/generated/images/src/__generated/image_icon_theme_images_navigation_cancel_40_40_E8F6FB.svg.d ./TouchGFX/generated/images/src/__generated/image_icon_theme_images_navigation_cancel_40_40_E8F6FB.svg.o ./TouchGFX/generated/images/src/__generated/image_icon_theme_images_navigation_cancel_40_40_E8F6FB.svg.su

.PHONY: clean-TouchGFX-2f-generated-2f-images-2f-src-2f-__generated

