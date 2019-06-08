COMPILER ?= aarch64-elf

DEBUG ?= 0
ifeq ($(DEBUG), 1)
    STOP = -s -S
else
    
endif

CPPOPS = -g -ffreestanding -Wall -Wextra -fno-exceptions -fno-rtti -fpermissive
COPS = -g -nostdlib -nostartfiles -ffreestanding
ASMOPS = -g -nostdlib -nostartfiles -ffreestanding 

BUILD_DIR = build/objects
BIN_DIR = build/bin

SRC_DIR = src

C_SRC_DIR = src/C
ASM_SRC_DIR = src/ASM
CPP_SRC_DIR = src/CPP

OBJ_DIR = objects
HEADER_DIR = include

all : build

.PHONY : clean
clean :
	rm -rf build

$(BUILD_DIR)/%.cpp.o: $(CPP_SRC_DIR)/%.cpp
	@echo "\n\n\n\e[32m"
	@echo "---------------------------------------------------------------------------------"
	@echo "STARTING C++ COMPILATION"
	@echo "---------------------------------------------------------------------------------"

	@mkdir -p $(@D)
	@$(COMPILER)-g++ $(CPPOPS) -I$(HEADER_DIR) -c $<  -o $@

$(BUILD_DIR)/%.c.o: $(C_SRC_DIR)/%.c
	@mkdir -p $(@D)
	@$(COMPILER)-gcc $(COPS) -I$(HEADER_DIR) -c $<  -o $@


$(BUILD_DIR)/%.asm.o: $(ASM_SRC_DIR)/%.S
	@mkdir -p $(@D)
	@$(COMPILER)-gcc $(ASMOPS) -I$(HEADER_DIR) -c $< -o $@


CPP_FILES = $(wildcard $(CPP_SRC_DIR)/*.cpp)
C_FILES = $(wildcard $(C_SRC_DIR)/*.c)
ASM_FILES = $(wildcard $(ASM_SRC_DIR)/*.S)

SOURCES := $(shell find $(SRC_DIR) -name '*.c')

OBJ_FILES = $(C_FILES:$(C_SRC_DIR)/%.c=$(BUILD_DIR)/%.c.o)
OBJ_FILES += $(ASM_FILES:$(ASM_SRC_DIR)/%.S=$(BUILD_DIR)/%.asm.o)
OBJ_FILES += $(CPP_FILES:$(CPP_SRC_DIR)/%.cpp=$(BUILD_DIR)/%.cpp.o)

HEADER_FILES += $(wildcard $(HEADER_DIR))


build: $(OBJ_FILES) $(HEADER_FILES)
	@echo "\e[34mC Files:"
	@echo $(C_FILES) | sed -e 's/ /\n/g'

	@echo "\e[35mC++ Files:"
	@echo $(CPP_FILES) | sed -e 's/ /\n/g'

	@echo "\e[36mC Files:"
	@echo $(OBJ_FILES) | sed -e 's/ /\n/g'
	

	@mkdir -p $(BIN_DIR)
	@$(COMPILER)-ld -g -T linker.ld -o $(BIN_DIR)/kernel8.elf  $(OBJ_FILES)

	@$(COMPILER)-objdump -S --disassemble $(BIN_DIR)/kernel8.elf > $(BIN_DIR)/kernel8.dump
	@$(COMPILER)-objcopy --only-keep-debug $(BIN_DIR)/kernel8.elf $(BIN_DIR)/kernel.sym
	@$(COMPILER)-objcopy --strip-debug $(BIN_DIR)/kernel8.elf
	@$(COMPILER)-objcopy $(BIN_DIR)/kernel8.elf -O binary build/kernel8.img

	@echo "\e[32m"
	@echo "---------------------------------------------------------------------------------"
	@echo "Build Success!"
	@echo "---------------------------------------------------------------------------------"

	@echo "\e[36mC Files:"
	@echo $(SOURCES) | sed -e 's/ /\n/g'

.PHONY : run
run : build
	qemu-system-aarch64 $(STOP) -m 1G -M raspi3 -serial stdio -kernel build/kernel8.img -d mmu

rebuild: clean build