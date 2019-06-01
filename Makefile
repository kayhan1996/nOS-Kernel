COMPILER ?= aarch64-elf

COPS = -ggdb -nostdlib -nostartfiles -ffreestanding 
ASMOPS = -ggdb -nostdlib -nostartfiles -ffreestanding 

BUILD_DIR = build/objects
BIN_DIR = build/bin

C_SRC_DIR = src/C
ASM_SRC_DIR = src/ASM

OBJ_DIR = objects
HEADER_DIR = include

all : build

.PHONY : clean
clean :
	rm -rf build

$(BUILD_DIR)/%.c.o: $(C_SRC_DIR)/%.c
	@mkdir -p $(@D)
	@$(COMPILER)-gcc $(COPS) -I$(HEADER_DIR) -c $<  -o $@


$(BUILD_DIR)/%.asm.o: $(ASM_SRC_DIR)/%.S
	@mkdir -p $(@D)
	@$(COMPILER)-gcc $(ASMOPS) -I$(HEADER_DIR) -c $< -o $@

C_FILES = $(wildcard $(C_SRC_DIR)/*.c)
ASM_FILES = $(wildcard $(ASM_SRC_DIR)/*.S)

OBJ_FILES = $(C_FILES:$(C_SRC_DIR)/%.c=$(BUILD_DIR)/%.c.o)
OBJ_FILES += $(ASM_FILES:$(ASM_SRC_DIR)/%.S=$(BUILD_DIR)/%.asm.o)

HEADER_FILES += $(wildcard $(HEADER_DIR))

build: $(OBJ_FILES) $(HEADER_FILES)
	@echo $(OBJ_FILES)
	@mkdir -p $(BIN_DIR)
	@$(COMPILER)-ld -g -T linker.ld -o $(BIN_DIR)/kernel8.elf  $(OBJ_FILES)

	@$(COMPILER)-objdump -S --disassemble $(BIN_DIR)/kernel8.elf > $(BIN_DIR)/kernel8.dump
	@$(COMPILER)-objcopy --only-keep-debug $(BIN_DIR)/kernel8.elf $(BIN_DIR)/kernel.sym
	@$(COMPILER)-objcopy --strip-debug $(BIN_DIR)/kernel8.elf
	@$(COMPILER)-objcopy $(BIN_DIR)/kernel8.elf -O binary build/kernel8.img
	@echo 	"BUILD SUCCESS"

.PHONY : run
run : build
	qemu-system-aarch64 -s -S -m 1G -M raspi3 -serial stdio -kernel build/kernel8.img -d mmu -d int -display none

rebuild: clean build