CC = clang
CC++ = clang++
OBJDUMP = objdump
OBJCOPY = objcopy

CPP_Flags =  --target=aarch64-none-elf \
			 -g \
			 -nostdlib -fno-builtin -nostartfiles -ffreestanding \
			 -fno-exceptions -fno-rtti -fpermissive \
			 -w

C_Flags = --target=aarch64-none-elf \
		  -g \
		  -nostdlib -fno-builtin -nostartfiles -ffreestanding\
		  -w

BUILD_DIR = build/objects
BIN_DIR = build/bin

SRC_DIR = src

C_SRC_DIR = src/C
ASM_SRC_DIR = src/ASM
CPP_SRC_DIR = src/CPP

OBJ_DIR = objects
HEADER_DIR = include

.PHONY : all
all : build

.PHONY : clean
clean :
	rm -rf build

$(BUILD_DIR)/%.cpp.o: $(CPP_SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	@$(CC++) $(CPP_Flags) -I$(HEADER_DIR) -c $<  -o $@

$(BUILD_DIR)/%.c.o: $(C_SRC_DIR)/%.c
	@mkdir -p $(@D)
	@$(CC) $(C_Flags) -I$(HEADER_DIR) -c $<  -o $@

$(BUILD_DIR)/%.asm.o: $(ASM_SRC_DIR)/%.S
	@mkdir -p $(@D)
	@$(CC) $(C_Flags) -I$(HEADER_DIR) -c $< -o $@

SOURCES := $(shell find $(SRC_DIR) -name '*.c')

CPP_FILES = $(wildcard $(CPP_SRC_DIR)/*.cpp)
C_FILES = $(wildcard $(C_SRC_DIR)/*.c)
ASM_FILES = $(wildcard $(ASM_SRC_DIR)/*.S)

OBJ_FILES = $(C_FILES:$(C_SRC_DIR)/%.c=$(BUILD_DIR)/%.c.o)
OBJ_FILES += $(ASM_FILES:$(ASM_SRC_DIR)/%.S=$(BUILD_DIR)/%.asm.o)
OBJ_FILES += $(CPP_FILES:$(CPP_SRC_DIR)/%.cpp=$(BUILD_DIR)/%.cpp.o)

HEADER_FILES += $(wildcard $(HEADER_DIR))

build/kernel8.img: $(OBJ_FILES) $(HEADER_FILES)
	@mkdir -p $(BIN_DIR)
	@ld.lld -g -T linker.ld -o $(BIN_DIR)/kernel8.elf $(OBJ_FILES) 

	$(OBJDUMP) -S --disassemble $(BIN_DIR)/kernel8.elf > $(BIN_DIR)/kernel8.dump
	$(OBJCOPY) --only-keep-debug $(BIN_DIR)/kernel8.elf build/kernel.sym
	$(OBJCOPY) --strip-debug $(BIN_DIR)/kernel8.elf
	$(OBJCOPY) $(BIN_DIR)/kernel8.elf -O binary build/kernel8.img
	

build: build/kernel8.img
	@echo "---------------------------------------------------------------------------------"
	@echo "Build Success!"
	@echo "---------------------------------------------------------------------------------"


.PHONY : run
run : build
	qemu-system-aarch64 -m 1G -M raspi3 -serial stdio -kernel build/kernel8.img -display none

rebuild: clean build