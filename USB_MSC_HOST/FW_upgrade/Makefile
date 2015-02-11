PROJECT=FW_Upgrade_Demo
EXECUTABLE=$(PROJECT).elf
BIN_IMAGE=$(PROJECT).bin
HEX_IMAGE=$(PROJECT).hex

# Toolchain configurations
CROSS_COMPILE ?= arm-none-eabi-
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
OBJCOPY = $(CROSS_COMPILE)objcopy                                                                           
OBJDUMP = $(CROSS_COMPILE)objdump
SIZE = $(CROSS_COMPILE)size


OUTPUT=./FW_Upgrade_Demo

SRC+=./../Libraries/CMSIS/Device/ST/STM32F4xx/Source/Templates/gcc_ride7/startup_stm32f429_439xx.s \
	./src/fattime.c \
	./../Utilities/Third_Party/fat_fs/src/ff.c \
	./../Libraries/STM32_USB_HOST_Library/Class/MSC/src/usbh_msc_bot.c \
	./../Libraries/STM32_USB_HOST_Library/Class/MSC/src/usbh_msc_core.c \
	./../Libraries/STM32_USB_HOST_Library/Class/MSC/src/usbh_msc_fatfs.c \
	./../Libraries/STM32_USB_HOST_Library/Class/MSC/src/usbh_msc_scsi.c \
	./../Libraries/STM32_USB_HOST_Library/Core/src/usbh_core.c \
	./../Libraries/STM32_USB_HOST_Library/Core/src/usbh_hcs.c \
	./../Libraries/STM32_USB_HOST_Library/Core/src/usbh_ioreq.c \
	./../Libraries/STM32_USB_HOST_Library/Core/src/usbh_stdreq.c \
	./../Libraries/STM32_USB_OTG_Driver/src/usb_core.c \
	./../Libraries/STM32_USB_OTG_Driver/src/usb_hcd.c \
	./../Libraries/STM32_USB_OTG_Driver/src/usb_hcd_int.c \
	./../Utilities/STM32F429I-Discovery/stm32f429i_discovery.c \
	./../Libraries/STM32F4xx_StdPeriph_Driver/src/misc.c \
	./../Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_adc.c \
	./../Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dac.c \
	./../Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dma.c \
	./../Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_exti.c \
	./../Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_flash.c \
	./../Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_gpio.c \
	./../Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_i2c.c \
	./../Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_rcc.c \
	./../Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_spi.c \
	./../Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_syscfg.c \
	./../Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_tim.c \
	./src/command.c \
	./src/flash_if.c \
	./src/main.c \
	./src/stm32f4xx_it.c \
	./src/system_stm32f4xx.c \
	./src/usb_bsp.c \
	./src/usbh_usr.c


CFLAGS_INCLUDE=-I. \
	-I./inc\
	-I./../Libraries/CMSIS/Device/ST/STM32F4xx/Include \
	-I./../Libraries/CMSIS/Include \
	-I./../Utilities/Third_Party/fat_fs/inc \
	-I./../Libraries/STM32F4xx_StdPeriph_Driver/inc \
	-I./../Libraries/STM32_USB_HOST_Library/Core/inc \
	-I./../Libraries/STM32_USB_HOST_Library/Class/MSC/inc \
	-I./../Libraries/STM32_USB_OTG_Driver/inc \
	-I./../Utilities/STM32F429I-Discovery

CFLAGS_DEFINE= \
	-D USE_STDPERIPH_DRIVER \
	-D STM32F429_439xx \
	-D USE_USB_OTG_HS \
	-D USE_EMBEDDED_PHY \
	-D __FPU_PRESENT=1 \
	-D ARM_MATH_CM4 \
	-D __FPU_USED=1 \
	-U printf -D printf=printf_base

#__CC_ARM
CFLAGS_NEW_LIB_NANO= \
	--specs=nano.specs --specs=nosys.specs -u _printf_float
CFLAGS_WARNING= \
	-Wall \
	-Wextra \
	-Wdouble-promotion \
	-Wshadow \
	-Werror=array-bounds \
	-Wfatal-errors \
	-Wmissing-prototypes \
	-Wbad-function-cast \
	-Wstrict-prototypes \
	-Wmissing-parameter-type
ARCH_FLAGS=-std=gnu99
ARCH_FLAGS+=-mlittle-endian -mthumb -mthumb-interwork -nostartfiles -mcpu=cortex-m4

CFLAGS=-g $(ARCH_FLAGS)\
${CFLAGS_INCLUDE} ${CFLAGS_DEFINE} \
${CFLAGS_WARNING}

CFLAGS+=-O3 -ffast-math
CFLAGS+=-ffunction-sections -fdata-sections
CFLAGS+=-Wl,--gc-sections
CFLAGS+=-fno-common
CFLAGS+=--param max-inline-insns-single=1000

ifeq ($(FLOAT_TYPE), hard)
CFLAGS += -fsingle-precision-constant -Wdouble-promotion
CFLAGS += -mfpu=fpv4-sp-d16 -mfloat-abi=hard
else
CFLAGS += -msoft-float
endif     

LDFLAGS +=$(CFLAGS_NEW_LIB_NANO) --static -Wl,--gc-sections
LDFLAGS += -T STM32F429ZI_FLASH.ld
LDLIBS +=-Wl,--start-group -lm -Wl,--end-group


all:$(BIN_IMAGE)
$(BIN_IMAGE):$(EXECUTABLE)
	@$(OBJCOPY) -O binary $^ $@
	@echo 'OBJCOPY $(BIN_IMAGE)'
$(EXECUTABLE): $(SRC)
	echo $(SRCS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(ARCH_FLAGS) $(SRC) $(LDLIBS) -o $@
	@echo 'LD $(EXECUTABLE)'
flash:
	st-flash write ./$(PROJECT).bin 0x8000000
clean:
	rm -f *.o
	rm -f ./$(PROJECT).elf
	rm -f ./$(PROJECT).hex
	rm -f ./$(PROJECT).bin
