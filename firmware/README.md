# Firmware Build Instructions

Bare-metal firmware for the STM32F411 (Nucleo-F411RE)

## Prerequisites

- `arm-none-eabi-gcc` toolchain (arm-none-eabi-gcc, -as, -ld, -objcopy)
- `make`
- [STM32CubeProgrammer](https://www.st.com/en/development-tools/stm32cubeprog.html) (for flashing)
- CMSIS / device headers from STM32CubeF4 (already in `inc/`, adjust path below if not)

## 1. Building manually with arm-none-eabi-gcc

This is what `make` runs under the hood - useful if you want to understand or
debug the build without the Makefile in the way.

```bash
# Compile each source file to an object file
arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard -DSTM32F411xE -I"inc" -c "src/main.c" -o main.o
arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard -DSTM32F411xE -I"inc" -c "src/lcd.c" -o lcd.o
arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard -DSTM32F411xE -I"inc" -c "src/uart.c" -o uart.o
arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard -DSTM32F411xE -I"inc" -c "inc/system_stm32f4xx.c" -o system_stm32f4xx.o

# Assemble the startup file
arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard -c startup/startup_stm32f411xe.s -o startup_stm32f411xe.o

# Link everything against the linker script
arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard --specs=nosys.specs -T"STM32F411RETX_FLASH.ld" main.o lcd.o uart.o system_stm32f4xx.o startup_stm32f411xe.o -o xp_lcd.elf

# Convert ELF to binary for flashing
arm-none-eabi-objcopy -O binary xp_lcd.elf xp_lcd.bin

# Flashing
STM32_Programmer_CLI.exe -c port=SWD -w xp_lcd.bin 0x08000000 -v -rst
```

Notes:

- `-specs=nosys.specs` is required because there's no OS underneath — without it you'll hit unresolved `_exit`/`_sbrk`/etc. symbols at link time.
- This project uses `-mfloat-abi=hard -mfpu=fpv4-sp-d16` to take advantage of the F411's hardware FPU. If you're porting this to a chip without an FPU, switch to `-mfloat-abi=soft` (or `softfp`) and drop the `-mfpu` flag — but note all object files linked together must use the same float ABI, or you'll get subtle bugs/link errors.


## 2. Building with make

The Makefile wraps all of the above. Day-to-day, you only need this:

```bash
# Remove build artifacts
make clean

# Build everything (produces build/firmware.elf/.bin)
make

# Flash directly (if the Makefile has a flash target wired to CubeProgrammer CLI)
make flash
```

If `make flash` isn't wired up yet, flash manually:

```bash
STM32_Programmer_CLI.exe -c port=SWD -w xplane-lcd-instruments.bin 0x08000000 -v -rst
```

- `-c port=SWD` connects over the ST-Link's SWD interface (built into the Nucleo board)
- `0x08000000` is the start of flash memory on the F411
- `-v` verifies after write, `-rst` resets the MCU to run the new firmware

## Troubleshooting

- **`region FLASH overflowed`** — check your linker script's memory map matches
  the F411's actual flash/RAM size (512KB flash / 128KB RAM on the CEUx variant)
- **CubeProgrammer can't find the ST-Link** — check the Nucleo is in normal mode
  (not DFU/bootloader) and drivers are installed
