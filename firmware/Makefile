
# ---- Toolchain ----
CC      = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
FLASHER = STM32_Programmer_CLI
 
# ---- Project files ----
TARGET     = xplane-lcd-instruments
INC_DIR    = inc
SRC_DIR    = src
STARTUP    = startup/startup_stm32f411xe.s
LDSCRIPT   = STM32F411RETX_FLASH.ld
 
# CMSIS system file lives in Inc/, everything else in Src/
SRCS = $(SRC_DIR)/main.c $(INC_DIR)/system_stm32f4xx.c $(SRC_DIR)/lcd.c $(SRC_DIR)/uart.c
 
# ---- Flags ----
CPU_FLAGS  = -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard
CFLAGS     = $(CPU_FLAGS) -DSTM32F411xE -I$(INC_DIR)
LDFLAGS    = $(CPU_FLAGS) --specs=nosys.specs -T$(LDSCRIPT)
 
OBJS = $(SRCS:.c=.o) startup.o
 
# ---- Rules ----
 
all: $(TARGET).bin
 
# compile any .c into its matching .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
 
startup.o: $(STARTUP)
	$(CC) $(CPU_FLAGS) -c $(STARTUP) -o startup.o
 
$(TARGET).elf: $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $(TARGET).elf
 
$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $(TARGET).elf $(TARGET).bin
 
flash: $(TARGET).bin
	$(FLASHER) -c port=SWD -w $(TARGET).bin 0x08000000 -v -rst
 
clean:
	del /Q $(SRC_DIR)\main.o $(INC_DIR)\system_stm32f4xx.o $(SRC_DIR)\lcd.o $(SRC_DIR)\uart.o startup.o $(TARGET).elf $(TARGET).bin 2>nul
 
.PHONY: all flash clean
