# Device Options
MCPU								:= cortex-m7

# Toolchain & Tools
TOOLCHAIN						?= arm-none-eabi
ST_FLASH						?= st-flash
GPP									:= $(TOOLCHAIN)-g++
GCC									:= $(TOOLCHAIN)-gcc
SIZE								:= $(TOOLCHAIN)-size
OBJCOPY							:= $(TOOLCHAIN)-objcopy
AS									:= $(TOOLCHAIN)-as

# General Files
LINKER_SCRIPT				:= ./linker.ld
FIRMWARE_ELF				:= firmware.elf
FIRMWARE_BIN				:= firmware.bin

# ST_FLASH Arguments
ST_FLASH_ARGS				+= write
ST_FLASH_ARGS				+= $(FIRMWARE_BIN)
ST_FLASH_ARGS				+= 0x8000000

# GCC Arguments
GCC_ARGS						+= -Wall
GCC_ARGS						+= -Werror

GCC_ARGS						+= -mthumb
GCC_ARGS						+= -mcpu=$(MCPU)

GCC_ARGS						+= -Os
GCC_ARGS						+= -ffunction-sections
GCC_ARGS						+= -fdata-sections
GCC_ARGS						+= -Wl,--gc-sections

GCC_ARGS 						+= -nostartfiles
GCC_ARGS						+= -nostdlib

GCC_ARGS						+= -I./Core/Inc
GCC_ARGS						+= -I./LWIP/App
GCC_ARGS						+= -I./LWIP/Target
GCC_ARGS						+= -I./Middlewares/Third_Party/LwIP/src/include
GCC_ARGS						+= -I./Middlewares/Third_Party/LwIP/system/
GCC_ARGS						+= -I./Drivers/CMSIS/Include
GCC_ARGS						+= -I./Drivers/CMSIS/Device/ST/STM32F7xx/Include
GCC_ARGS						+= -I./Drivers/STM32F7xx_HAL_Driver/Inc
GCC_ARGS						+= -I../shared/include

GCC_ARGS						+= -mfloat-abi=hard
GCC_ARGS						+= -DSTM32F767xx

LD_ARGS							+= -mcpu=$(MCPU)
LD_ARGS							+= -ffunction-sections
LD_ARGS							+= -fdata-sections
LD_ARGS							+= -Wl,--gc-sections
LD_ARGS							+= -mfloat-abi=hard

# AS Arguments
AS_ARGS							+= -mthumb
AS_ARGS							+= -mfloat-abi=hard
AS_ARGS							+= -mcpu=$(MCPU)

# GPP Arguments
GPP_ARGS						+= $(GCC_ARGS)

# OBJCOPY Arguments
OBJCOPY_ARGS				+= -O binary
OBJCOPY_ARGS				+= $(FIRMWARE_ELF)
OBJCOPY_ARGS				+= $(FIRMWARE_BIN)

# Size Arguments
SIZE_ARGS						+= --format=gnu
SIZE_ARGS						+= --radix=10
SIZE_ARGS						+= --common
SIZE_ARGS						+= $(FIRMWARE_ELF)

# Files
GPP_SOURCES						+= $(shell find ./Core/Src -name *.cc)
C_SOURCES						+= $(shell find ./Core/Src -name *.c)
C_SOURCES						+= $(shell find ./LWIP -name *.c)
C_SOURCES						+= $(shell find ./Middlewares/Third_Party/LwIP/src -name *.c)
C_SOURCES						+= $(shell find ./Drivers/ -name *.c ! -name *_template.c)
S_SOURCES						+= $(shell find ./Core/Src -name *.s)

OBJECTS							+= $(GPP_SOURCES:.cc=.arm.o)
OBJECTS							+= $(C_SOURCES:.c=.arm.o)
OBJECTS							+= $(S_SOURCES:.s=.arm.o)

# Compilation
%.arm.o: %.s
	$(AS) $(AS_ARGS) $< -o $@
%.arm.o: %.cc
	$(GPP) $(GPP_ARGS) -c $< -o $@
%.arm.o: %.c
	$(GCC) $(GCC_ARGS) -c $< -o $@

# General Make Rules
all: $(OBJECTS)
	$(GPP) $(OBJECTS) $(LD_ARGS) -T $(LINKER_SCRIPT) -o $(FIRMWARE_ELF)
size:
	$(SIZE) $(SIZE_ARGS)
clean:
	rm -rf $(OBJECTS) firmware.bin firmware.elf
bin:
	$(OBJCOPY) $(OBJCOPY_ARGS)
flash:
	$(ST_FLASH) $(ST_FLASH_ARGS)
