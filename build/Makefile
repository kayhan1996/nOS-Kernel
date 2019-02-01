COMPILER = aarch64-elf-gcc
LINKER = aarch64-elf-ld
CFLAGS = -ffreestanding -nostdinc -nostdlib -nostartfiles
LFLAGS = -nostdlib -nostartfiles

KERNEL_SRC = ../src/kernel
KERNEL_HEADERS = ../include
OBJ_DIR = objects

KERNEL_SOURCES = $(wildcard $(KERNEL_SRC)/*.c)
ASM_SOURCES = $(wildcard $(KERNEL_SRC)/.S)

OBJECTS = $(patsubst $(KERNEL_SRC)/%.c, $(OBJ_DIR)/%.o, $(KERNEL_SOURCES))
OBJECTS += $(patsubst $(KERNEL_SRC)/%.S, $(OBJ_DIR)/%.o, $(ASM_SOURCES))
HEADERS = $(wildcard $(KERNEL_HEADERS)/*h)

IMG_NAME = kernel8.img

build: $(OBJECTS) $(HEADERS)
	echo	$(OBJECTS)
	echo	"Bravo"
	$(LINKER) -T linker.ld -o $(IMG_NAME) $(LFLAGS) $(OBJECTS)

$(OBJ_DIR)/%.o: $(KER_SRC)/%.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -I$(KERNEL_SRC) -c $< -o $@ $(CFLAGS)

$(OBJ_DIR)/%.o: $(KERNEL_SRC)/%.S
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -I$(KERNEL_SRC) -c $< -o $@
